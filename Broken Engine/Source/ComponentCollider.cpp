#include "ComponentCollider.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentDynamicRigidBody.h"
#include "ModulePhysics.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "OpenGL.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ComponentCollider::ComponentCollider(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Collider)
{
	name = "Collider";
	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultColliderMesh");
}

ComponentCollider::~ComponentCollider()
{
	mesh->Release();
}

void ComponentCollider::Update()
{
	if (editCollider) {
		UpdateCollider();
	}

	if (to_delete)
	{
		Delete();
		this->GetContainerGameObject()->RemoveComponent(this);
	}
}

void ComponentCollider::UpdateCollider() {
	CreateCollider((ComponentCollider::COLLIDER_TYPE)colliderType, true);
}

void ComponentCollider::DrawComponent()
{
	if (shape)
	{
		if (GetActor() != nullptr)
		{
			if (!GetActive())
			{
				GetActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
				if (!hasBeenDeactivated)
				{
					App->physics->DeleteActor(GetActor());
					hasBeenDeactivated = true;
				}
			}
			else
			{
				if (hasBeenDeactivated)
				{
					CreateCollider(type, true);
					hasBeenDeactivated = false;
				}

				GetActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
			}
		}


		// --- Get shape's dimensions ---
		physx::PxGeometryHolder holder = shape->getGeometry();
		physx::PxGeometryType::Enum type = holder.getType();

		if (!mesh->IsInMemory())
		{
			// --- Draw shape according to type ---
			switch (type)
			{
			case physx::PxGeometryType::eSPHERE:
			{
				physx::PxSphereGeometry pxsphere = holder.sphere();

				// --- Rebuild sphere ---
				App->scene_manager->CreateSphere(1, 25, 25, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::ePLANE:
			{
				physx::PxPlaneGeometry pxplane = holder.plane();

				// --- Rebuild plane ---
				App->scene_manager->CreatePlane(1, 1, 1, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::eCAPSULE:
			{
				physx::PxCapsuleGeometry capsule = holder.capsule();

				// --- Rebuild capsule ---
				App->scene_manager->CreateCapsule(radius, height, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::eBOX:
			{
				physx::PxCapsuleGeometry capsule = holder.capsule();

				// --- Rebuild box ---
				App->scene_manager->CreateCube(1, 1, 1, mesh);
				mesh->LoadToMemory();

			}
			break;
			case physx::PxGeometryType::eCONVEXMESH:
				break;
			case physx::PxGeometryType::eTRIANGLEMESH:
				break;
			case physx::PxGeometryType::eHEIGHTFIELD:
				break;
			case physx::PxGeometryType::eGEOMETRY_COUNT:
				break;
			case physx::PxGeometryType::eINVALID:
				break;
			default:
				break;
			}
		}
	}

	// --- Render shape ---
	if (mesh && mesh->IsInMemory() && mesh->vertices && mesh->Indices && App->GetAppState() != AppState::PLAY)
	{
		RenderMeshFlags flags = wire;
		App->renderer3D->DrawMesh(globalMatrix, mesh, (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID(), false), nullptr, flags, Color(125,125,125));
	}

}

void ComponentCollider::UpdateLocalMatrix() {

	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();
	ComponentTransform* cTransform = GO->GetComponent<ComponentTransform>();

	if (!rigidStatic && !dynamicRB)
		return;

	math::float4x4 gt = cTransform->GetGlobalTransform();

	//Render
	localMatrix.x = centerPosition.x + offset.x;
	localMatrix.y = centerPosition.y + offset.y;
	localMatrix.z = centerPosition.z + offset.z;

	localMatrix.scaleX = colliderSize.x * originalSize.x;
	localMatrix.scaleY = colliderSize.y * originalSize.y;
	localMatrix.scaleZ = colliderSize.z * originalSize.z;

	globalMatrix = gt * localMatrix;

	//PHYSX DEBUG
	float3 pos, scale;
	Quat rot;
	globalMatrix.Decompose(pos, rot, scale);

	float threshold = 0.1f;

	if ((math::Abs(scale.x - tmpScale.x) > threshold
		|| math::Abs(scale.y - tmpScale.y) > threshold
		|| math::Abs(scale.z - tmpScale.z) > threshold))
	{
		editCollider = true;
		tmpScale = scale;
	}

	physx::PxVec3 posi(pos.x, pos.y, pos.z);
	physx::PxQuat quati(rot.x, rot.y, rot.z, rot.w);
	physx::PxTransform transform(posi, quati);


	if (dynamicRB == nullptr)
		rigidStatic->setGlobalPose(transform); //ON EDITOR
	else
	{
		if ((App->gui->isUsingGuizmo && !App->isGame) || cTransform->updateValues){ //ON EDITOR
				dynamicRB->rigidBody->setGlobalPose(transform);
		}
		else if (dynamicRB->rigidBody != nullptr) //ON GAME
		{
			UpdateTransformByRigidBody(dynamicRB, cTransform);
		}
	}
}

void ComponentCollider::UpdateTransformByRigidBody(ComponentDynamicRigidBody* RB, ComponentTransform* cTransform, physx::PxTransform* globalPos) {
	physx::PxTransform transform;
	if (!RB)
		return;

	if (globalPos) {
		transform = physx::PxTransform(globalPos->p, globalPos->q);
		RB->rigidBody->setGlobalPose(transform);
	}

	std::string name = GO->GetName();
	transform = RB->rigidBody->getGlobalPose();
	float x = transform.p.x - offset.x;

	bool isFalling = RB->rigidBody->getLinearVelocity().y != 0.0f;

	if (isFalling)
		cTransform->SetPosition(transform.p.x - offset.x, transform.p.y - offset.y - localMatrix.y, transform.p.z - offset.z);

	cTransform->SetRotation(Quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
	globalMatrix = cTransform->GetGlobalTransform() * localMatrix;

	if (App->GetAppState() == AppState::PLAY && !toPlay)
	{
		float3 pos, scale;
		Quat rot;
		globalMatrix.Decompose(pos, rot, scale);

		physx::PxVec3 posi(pos.x, pos.y, pos.z);
		physx::PxQuat quati(rot.x, rot.y, rot.z, rot.w);
		physx::PxTransform transform(posi, quati);

		RB->rigidBody->setGlobalPose(transform);

		toPlay = true;
	}
}

json ComponentCollider::Save() const
{
	ENGINE_CONSOLE_LOG("Saved");
	json node;

	node["Active"] = this->active;

	int colliderType = 0;
	switch (type)
	{
	case COLLIDER_TYPE::NONE:
		colliderType = 0;
			break;
	case COLLIDER_TYPE::BOX:
		colliderType = 1;
		break;
	case COLLIDER_TYPE::SPHERE:
		colliderType = 2;
		break;
	case COLLIDER_TYPE::CAPSULE:
		colliderType = 3;
		break;
	case COLLIDER_TYPE::PLANE:
		colliderType = 4;
		break;
	}

	node["colliderType"] = std::to_string(colliderType);
	node["isTrigger"] = std::to_string(isTrigger);

	node["localPositionx"] = std::to_string(centerPosition.x);
	node["localPositiony"] = std::to_string(centerPosition.y);
	node["localPositionz"] = std::to_string(centerPosition.z);

	node["originalScalex"] = std::to_string(originalSize.x);
	node["originalScaley"] = std::to_string(originalSize.y);
	node["originalScalez"] = std::to_string(originalSize.z);

	node["offsetx"] = std::to_string(offset.x);
	node["offsety"] = std::to_string(offset.y);
	node["offsetz"] = std::to_string(offset.z);

	node["globalPositionx"] = std::to_string(originalSize.x);
	node["globalPositiony"] = std::to_string(originalSize.y);
	node["globalPositionz"] = std::to_string(originalSize.z);

	node["localMatrixx"] = std::to_string(localMatrix.x);
	node["localMatrixy"] = std::to_string(localMatrix.y);
	node["localMatrixz"] = std::to_string(localMatrix.z);
	node["localMatrixw"] = std::to_string(localMatrix.w);

	node["globalMatrixx"] = std::to_string(globalMatrix.x);
	node["globalMatrixy"] = std::to_string(globalMatrix.y);
	node["globalMatrixz"] = std::to_string(globalMatrix.z);
	node["globalMatrixw"] = std::to_string(globalMatrix.w);

	node["scalex"] = std::to_string(colliderSize.x);
	node["scaley"] = std::to_string(colliderSize.y);
	node["scalez"] = std::to_string(colliderSize.z);

	node["radius"] = std::to_string(radius);

	node["height"] = std::to_string(height);

	node["lastIndex"] = std::to_string(lastIndex);

	node["colliderType"] = std::to_string(colliderType);

	node["tmpScalex"] = std::to_string(tmpScale.x);
	node["tmpScaley"] = std::to_string(tmpScale.y);
	node["tmpScalez"] = std::to_string(tmpScale.z);

	node["firstCreation"] = std::to_string(firstCreation);

	if (hasBeenDeactivated)
		node["hasBeenDeactivated"] = std::to_string(1);
	else
		node["hasBeenDeactivated"] = std::to_string(0);


	return node;
}

void ComponentCollider::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];

	ENGINE_CONSOLE_LOG("Load");
	//CreateCollider(COLLIDER_TYPE::NONE, true);

	std::string localPositionx = node["localPositionx"].is_null() ? "0" : node["localPositionx"];
	std::string localPositiony = node["localPositiony"].is_null() ? "0" : node["localPositiony"];
	std::string localPositionz = node["localPositionz"].is_null() ? "0" : node["localPositionz"];

	std::string originalScalex = node["originalScalex"].is_null() ? "0" : node["originalScalex"];
	std::string originalScaley = node["originalScaley"].is_null() ? "0" : node["originalScaley"];
	std::string originalScalez = node["originalScalez"].is_null() ? "0" : node["originalScalez"];

	std::string offsetx = node["offsetx"].is_null() ? "0" : node["offsetx"];
	std::string offsety = node["offsety"].is_null() ? "0" : node["offsety"];
	std::string offsetz = node["offsetz"].is_null() ? "0" : node["offsetz"];

	std::string localMatrixx = node["localMatrixx"].is_null() ? "0" : node["localMatrixx"];
	std::string localMatrixy = node["localMatrixy"].is_null() ? "0" : node["localMatrixy"];
	std::string localMatrixz = node["localMatrixz"].is_null() ? "0" : node["localMatrixz"];
	std::string localMatrixw = node["localMatrixw"].is_null() ? "0" : node["localMatrixw"];

	std::string globalMatrixx = node["globalMatrixx"].is_null() ? "0" : node["globalMatrixx"];
	std::string globalMatrixy = node["globalMatrixy"].is_null() ? "0" : node["globalMatrixy"];
	std::string globalMatrixz = node["globalMatrixz"].is_null() ? "0" : node["globalMatrixz"];
	std::string globalMatrixw = node["globalMatrixw"].is_null() ? "0" : node["globalMatrixw"];

	std::string scalex = node["scalex"].is_null() ? "0" : node["scalex"];
	std::string scaley = node["scaley"].is_null() ? "0" : node["scaley"];
	std::string scalez = node["scalez"].is_null() ? "0" : node["scalez"];

	std::string radius_ = node["radius"].is_null() ? "0" : node["radius"];

	std::string height_ = node["height"].is_null() ? "0" : node["height"];

	std::string lastIndex_ = node["lastIndex"].is_null() ? "0" : node["lastIndex"];

	std::string colliderType_ = node["colliderType"].is_null() ? "0" : node["colliderType"];

	std::string isTrigger_ = node["isTrigger"].is_null() ? "0" : node["isTrigger"];

	std::string tmpScalex = node["tmpScalex"].is_null() ? "0" : node["tmpScalex"];
	std::string tmpScaley = node["tmpScaley"].is_null() ? "0" : node["tmpScaley"];
	std::string tmpScalez = node["tmpScalez"].is_null() ? "0" : node["tmpScalez"];

	std::string firstCreation_ = node["firstCreation"].is_null() ? "0" : node["firstCreation"];

	std::string hasBeenDeactivated_ = node["hasBeenDeactivated"].is_null() ? "0" : node["hasBeenDeactivated"];


	centerPosition = float3(std::stof(localPositionx), std::stof(localPositiony), std::stof(localPositionz));
	originalSize = float3(std::stof(originalScalex), std::stof(originalScaley), std::stof(originalScalez));
	offset = float3(std::stof(offsetx), std::stof(offsety), std::stof(offsetz));

	localMatrix.x = std::stof(localMatrixx);
	localMatrix.y = std::stof(localMatrixy);
	localMatrix.z = std::stof(localMatrixz);
	localMatrix.w = std::stof(localMatrixw);

	globalMatrix.x = std::stof(globalMatrixx);
	globalMatrix.y = std::stof(globalMatrixy);
	globalMatrix.z = std::stof(globalMatrixz);
	globalMatrix.w = std::stof(globalMatrixw);

	colliderSize = float3(std::stof(scalex), std::stof(scaley), std::stof(scalez));

	radius = std::stof(radius_);
	height = std::stof(height_);
	lastIndex = std::stoi(lastIndex_);
	colliderType = std::stoi(colliderType_);
	isTrigger = std::stoi(isTrigger_);

	//tmpScale = float3(std::stof(tmpScalex), std::stof(tmpScaley), std::stof(tmpScalez));
	tmpScale = float3(0.f, 0.f, 0.f);

	firstCreation = true;

	/*if (hasBeenDeactivated_ == "1")
		hasBeenDeactivated = true;
	else*/
	//hasBeenDeactivated = true;

	toPlay = false;

	std::string colliderEnum = node["colliderType"];

	lastIndex = 0;

	int tmp = std::stoi(colliderEnum);
	colliderType = tmp;
	switch (tmp)
	{
	case 0:
		type = COLLIDER_TYPE::NONE;
		CreateCollider(type);
		break;
	case 1:
		type = COLLIDER_TYPE::BOX;
		CreateCollider(type, true);
		break;
	case 2:
		type = COLLIDER_TYPE::SPHERE;
		CreateCollider(type);
		break;
	case 3:
		type = COLLIDER_TYPE::CAPSULE;
		CreateCollider(type);
	case 4:
		type = COLLIDER_TYPE::PLANE;
		CreateCollider(type);
		break;
	}

	UpdateLocalMatrix();
}

void ComponentCollider::CreateInspectorNode()
{
	bool createAgain = false;

	if (ImGui::Button("Delete component"))
		to_delete = true;

	if (ImGui::Combo("Type", &colliderType, "NONE\0BOX\0SPHERE\0CAPSULE\0\0")) 
	{
		switch (colliderType)
		{
		case 0:
			type = ComponentCollider::COLLIDER_TYPE::NONE;
			break;
		case 1:
			type = ComponentCollider::COLLIDER_TYPE::BOX;
			break;
		case 2:
			type = ComponentCollider::COLLIDER_TYPE::SPHERE;
			break;
		case 3:
			type = ComponentCollider::COLLIDER_TYPE::CAPSULE;
			break;
		}
		editCollider = true;
	}

	if (shape)
	{
		ImGui::Text("Is Trigger");
		ImGui::SameLine();
		if (ImGui::Checkbox("##T", &isTrigger))
			editCollider = true;

		float3* position = &centerPosition;

		ImGui::Text("Center");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PX", &position->x, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PY", &position->y, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PZ", &position->z, 0.005f);

		switch (shape->getGeometryType())
		{
		case physx::PxGeometryType::eSPHERE:
		{
			float prevRadius = radius;

			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##R", &radius, 0.005f);

			colliderSize.x = radius;
			colliderSize.y = radius;
			colliderSize.z = radius;

			if (prevRadius != radius || editCollider) {
				editCollider = true;
				colliderType = (int)COLLIDER_TYPE::SPHERE;
			}

			break;
		}

		case physx::PxGeometryType::eBOX:
		{
			float3 prevScale = colliderSize;
			ImGui::Text("Size");
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

			ImGui::DragFloat("##SX", &colliderSize.x, 0.005f, 0.01f, 1000.0f);

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

			ImGui::DragFloat("##SY", &colliderSize.y, 0.005f, 0.01f, 1000.0f);

			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

			ImGui::DragFloat("##SZ", &colliderSize.z, 0.005f, 0.01f, 1000.0f);

			if (prevScale.x != colliderSize.x || prevScale.y != colliderSize.y || prevScale.z != colliderSize.z || editCollider) {
				editCollider = true;
				colliderType = (int)COLLIDER_TYPE::BOX;
			}

			break;
		}

		case physx::PxGeometryType::eCAPSULE:
		{
			float prevRadius = radius;
			float prevheight = height;

			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##R", &radius, 0.005f);

			ImGui::Text("Height");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("##H", &height, 0.005f);

			colliderSize.x = radius;
			colliderSize.y = height;
			colliderSize.z = radius;

			if (prevRadius != radius || prevheight != height || editCollider) {
				editCollider = true;
				colliderType = (int)COLLIDER_TYPE::CAPSULE;
			}

			break;
		}

		}
	}
}


void ComponentCollider::CreateCollider(ComponentCollider::COLLIDER_TYPE type, bool createAgain) {
	if (shape != nullptr && (lastIndex != (int)type || createAgain)) {
		shape->release();
		shape = nullptr;
		if (GO->GetComponent<ComponentDynamicRigidBody>() != nullptr)
		{
			if (GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody != nullptr)
				App->physics->DeleteActor(GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody);
			if (createAgain && rigidStatic)
				App->physics->DeleteActor(rigidStatic);
		}
		else
			App->physics->DeleteActor(rigidStatic);


		// --- Make sure to always enter here or else the mesh's data won't be released!!! ---
		if (mesh && mesh->IsInMemory())
		{
			mesh->Release();
		}
	}

	if (lastIndex == (int)type && !createAgain) {
		return;
	}

	physx::PxTransform localTransform(physx::PxVec3(centerPosition.x, centerPosition.y, centerPosition.z));
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	float3 tScale = transform->GetScale();

	float3 pos, scale;
	Quat quat;
	transform->GetGlobalTransform().Decompose(pos, quat, scale);

	switch (type) {
		case ComponentCollider::COLLIDER_TYPE::BOX: {

			float3 center;

			physx::PxBoxGeometry boxGeometry;
			Quat q = transform->rotation;
			Quat qInverse = q;
			if (!firstCreation)
			{
				qInverse.InverseAndNormalize();
				Quat tmp = q * qInverse;
				transform->rotation = tmp;

				transform->SetRotation(transform->rotation);
				transform->UpdateLocalTransform();

				GO->UpdateAABB();

				originalSize = GO->GetOBB().Size().Div(scale);

				center = GO->GetAABB().CenterPoint();

			}

			boxGeometry = physx::PxBoxGeometry(physx::PxVec3(originalSize.x * scale.x * colliderSize.x * 0.5, originalSize.y * scale.y * colliderSize.y * 0.5, originalSize.z * scale.z * colliderSize.z * 0.5));

			physx::PxTransform position(physx::PxVec3(center.x, center.y, center.z));

			shape = App->physics->mPhysics->createShape(boxGeometry, *App->physics->mMaterial);
			shape->setGeometry(boxGeometry);

			if (!HasDynamicRigidBody(boxGeometry, position))
			{
				if(rigidStatic)
					App->physics->DeleteActor(rigidStatic);

				physx::PxFilterData filterData;
				filterData.word0 = (1 << GO->layer); // word0 = own ID
				filterData.word1 = App->physics->layer_list.at(GO->layer).LayerGroup; // word1 = ID mask to filter pairs that trigger a contact callback;

				if (isTrigger) {
					shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
					shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
				}
				else {
					shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
					shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
				}
				physx::PxShapeFlags falgs =  shape->getFlags();


				shape->setSimulationFilterData(filterData);
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				shape->setQueryFilterData(filterData);


				bool a;
				a = falgs.isSet(physx::PxShapeFlag::ePARTICLE_DRAIN);
				a = falgs.isSet(physx::PxShapeFlag::eSCENE_QUERY_SHAPE);
				a = falgs.isSet(physx::PxShapeFlag::eSIMULATION_SHAPE);
				a = falgs.isSet(physx::PxShapeFlag::eTRIGGER_SHAPE);

				rigidStatic = PxCreateStatic(*App->physics->mPhysics, position, *shape);

				App->physics->addActor(rigidStatic, GO);
			}



			if (!firstCreation)
			{
				transform->SetRotation(q); //RESET TO ORIGNAL ROTATION

				GO->UpdateAABB();
				firstCreation = true;

				center = GO->GetAABB().CenterPoint();
				float3 dir = center - transform->GetGlobalPosition();
				float3 dir2 = quat.Inverted().Mul(dir); // rotate it
				offset = (dir2.Div(scale));

				offset.Mul(scale);
			}

			UpdateLocalMatrix();

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::BOX;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::SPHERE: {
			physx::PxSphereGeometry SphereGeometry(radius);
			shape = App->physics->mPhysics->createShape(SphereGeometry, *App->physics->mMaterial);
			shape->setGeometry(SphereGeometry);

			if (!HasDynamicRigidBody(SphereGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->addActor(rigidStatic, GO);
			}

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::SPHERE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::PLANE: {
			physx::PxBoxGeometry planeGeometry(physx::PxVec3(colliderSize.x, 0.0001f, colliderSize.z));
			shape = App->physics->mPhysics->createShape(planeGeometry, *App->physics->mMaterial);
			shape->setGeometry(planeGeometry);

			if (!HasDynamicRigidBody(planeGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->addActor(rigidStatic, GO);
			}

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::PLANE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::CAPSULE: {
			physx::PxCapsuleGeometry CapsuleGeometry(radius, height);
			shape = App->physics->mPhysics->createShape(CapsuleGeometry, *App->physics->mMaterial);
			shape->setGeometry(CapsuleGeometry);

			if (!HasDynamicRigidBody(CapsuleGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->addActor(rigidStatic, GO);
			}

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::CAPSULE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::NONE: {
			lastIndex = -1;
			break;
		}
	}
	editCollider = false;
}

void ComponentCollider::Delete()
{
	if (shape)
	{
		shape->release();
		shape = nullptr;
	}

	if (GO->GetComponent<ComponentDynamicRigidBody>() != nullptr)
	{
		if (GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody != nullptr)
			App->physics->mScene->removeActor(*(physx::PxActor*)GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody);
		if (rigidStatic)
			App->physics->mScene->removeActor(*(physx::PxActor*)rigidStatic);
	}

	else if(rigidStatic)
		App->physics->mScene->removeActor(*(physx::PxActor*)rigidStatic);
}

template <class Geometry>
bool ComponentCollider::HasDynamicRigidBody(Geometry geometry, physx::PxTransform transform) const
{
	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();

	if (dynamicRB != nullptr)
	{

		float3 position, scale = float3::zero;
		Quat rot = Quat::identity;

		globalMatrix.Decompose(position, rot, scale);

		dynamicRB->rigidBody = PxCreateDynamic(*App->physics->mPhysics, transform, geometry, *App->physics->mMaterial, 1.0f);

		physx::PxShape* shape_;
		dynamicRB->rigidBody->getShapes(&shape_, 1);
		if (shape_) {
			*shape = *shape_;
			if (isTrigger) {
				shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
				shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			}
			else {
				shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
				shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
			}
		}

		App->physics->setupFiltering((physx::PxRigidActor*)dynamicRB->rigidBody, (1 << GO->layer), App->physics->layer_list.at(GO->layer).LayerGroup); //Setup filtering Layers

		dynamicRB->rigidBody->setGlobalPose(physx::PxTransform(position.x,position.y,position.z, physx::PxQuat(rot.x, rot.y, rot.z, rot.w)));

		if (rigidStatic) {
			App->physics->DeleteActor(rigidStatic);
		}
		else if (dynamicRB->rigidBody) {
			App->physics->DeleteActor(dynamicRB->rigidBody);
		}

		App->physics->addActor(dynamicRB->rigidBody, GO);

		return true;
	}

	else {
		return false;
	}
}

physx::PxRigidActor* ComponentCollider::GetActor() {
	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();

	if (dynamicRB != nullptr)
		return dynamicRB->rigidBody;
	else
		return rigidStatic;
}

void ComponentCollider::UpdateActorLayer(LayerMask* layerMask) {
	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();

	if (dynamicRB != nullptr)
		App->physics->UpdateActorLayer(dynamicRB->rigidBody, layerMask);
	else
		App->physics->UpdateActorLayer(rigidStatic, layerMask);
}
