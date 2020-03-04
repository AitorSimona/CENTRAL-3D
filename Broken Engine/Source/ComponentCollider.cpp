#include "ComponentCollider.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentDynamicRigidBody.h"
#include "ModulePhysics.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "OpenGL.h"

#include "Imgui/imgui.h"
#include "ImGUi/ImGuizmo/ImGuizmo.h"

#include "mmgr/mmgr.h"

using namespace physx;

ComponentCollider::ComponentCollider(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Collider)
{
	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultColliderMesh");
}

ComponentCollider::~ComponentCollider()
{
	mesh->Release();
}

void ComponentCollider::Draw() 
{
	if (App->GetAppState() == AppState::TO_EDITOR)
	{
		ENGINE_CONSOLE_LOG("Deleted");
		Delete();
	}

	if (shape)
	{
		// --- Get shape's dimensions ---
		PxGeometryHolder holder = shape->getGeometry();
		PxGeometryType::Enum type = holder.getType();

		if (!mesh->IsInMemory())
		{
			// --- Draw shape according to type ---
			switch (type)
			{
			case physx::PxGeometryType::eSPHERE:
			{
				PxSphereGeometry pxsphere = holder.sphere();

				// --- Rebuild capsule ---
				App->scene_manager->CreateSphere(1, 25, 25, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::ePLANE:
			{
				PxPlaneGeometry pxplane = holder.plane();

				// --- Rebuild plane ---
				App->scene_manager->CreatePlane(1, 1, 1, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::eCAPSULE:
			{
				PxCapsuleGeometry capsule = holder.capsule();

				// --- Rebuild capsule ---
				App->scene_manager->CreateCapsule(1, 1, mesh);
				mesh->LoadToMemory();
			}
			break;
			case physx::PxGeometryType::eBOX:
			{
				PxCapsuleGeometry capsule = holder.capsule();

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
		if (mesh && mesh->IsInMemory() && mesh->vertices && mesh->Indices)
		{
			// --- Use default shader ---
			glUseProgram(App->renderer3D->defaultShader->ID);

			// --- Set uniforms ---
			GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");

			//UpdateLocalMatrix();

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, globalMatrix.Transposed().ptr());

			int vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
			glUniform3f(vertexColorLocation, 125, 125, 125);

			int TextureSupportLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
			glUniform1i(TextureSupportLocation, -1);

			// --- Bind mesh's vao ---
			glBindVertexArray(mesh->VAO);

			// --- Activate wireframe mode ---
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// --- bind indices buffer and draw ---
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
			glDrawElements(GL_TRIANGLES, mesh->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

			// --- Unbind mesh's vao ---
			glBindVertexArray(0);

			// --- DeActivate wireframe mode ---
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// --- Set uniforms back to defaults ---
			glUniform1i(TextureSupportLocation, 0);
			glUniform3f(vertexColorLocation, 255, 255, 255);
		}
	
}

void ComponentCollider::UpdateLocalMatrix() {

	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();
	ComponentTransform* cTransform = GO->GetComponent<ComponentTransform>();

	if (!rigidStatic && !dynamicRB)
		return;

	//Render
	localMatrix.x = localPosition.x + offset.x;
	localMatrix.y = localPosition.y + offset.y;
	localMatrix.z = localPosition.z + offset.z;
	localMatrix.scaleX = scale.x * originalScale.x; //scale * sizeAABB
	localMatrix.scaleY = scale.y * originalScale.y;
	localMatrix.scaleZ = scale.z * originalScale.z;

	math::float4x4 gt = cTransform->GetGlobalTransform();
	globalMatrix = gt * localMatrix;

	//PHYSX DEBUG
	float3 pos, scale;
	Quat rot;
	globalMatrix.Decompose(pos, rot, scale);
	scale = cTransform->GetScale();

	if (!scale.Equals(tmpScale)) {
		editCollider = true;
		tmpScale = scale;
	}

	PxVec3 posi(pos.x, pos.y, pos.z);
	PxQuat quati(rot.x, rot.y, rot.z, rot.w);
	PxTransform transform(posi, quati);

	if (!dynamicRB)
		rigidStatic->setGlobalPose(transform); //ON EDITOR
	else
	{
		if (ImGuizmo::IsUsing() || cTransform->updateValues) { //ON EDITOR
			dynamicRB->rigidBody->setGlobalPose(transform);
		}
		else {
			if (dynamicRB->rigidBody != nullptr) //ON GAME
			{
				UpdateTransformByRigidBody(dynamicRB, cTransform);
			}
		}
	}
}

void ComponentCollider::UpdateTransformByRigidBody(ComponentDynamicRigidBody* RB, ComponentTransform* cTransform, physx::PxTransform* globalPos) {	
	
	PxTransform transform;
	if (!RB)
		return;

	if (globalPos) {
		transform = PxTransform(globalPos->p, globalPos->q);
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

		PxVec3 posi(pos.x, pos.y, pos.z);
		PxQuat quati(rot.x, rot.y, rot.z, rot.w);
		PxTransform transform(posi, quati);

		RB->rigidBody->setGlobalPose(transform);

		toPlay = true;
	}
}

json ComponentCollider::Save() const
{
	ENGINE_CONSOLE_LOG("Saved");
	json node;

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

	node["localPositionx"] = std::to_string(localPosition.x);
	node["localPositiony"] = std::to_string(localPosition.y);
	node["localPositionz"] = std::to_string(localPosition.z);

	node["originalScalex"] = std::to_string(originalScale.x);
	node["originalScaley"] = std::to_string(originalScale.y);
	node["originalScalez"] = std::to_string(originalScale.z);

	node["offsetx"] = std::to_string(offset.x);
	node["offsety"] = std::to_string(offset.y);
	node["offsetz"] = std::to_string(offset.z);

	node["globalPositionx"] = std::to_string(originalScale.x);
	node["globalPositiony"] = std::to_string(originalScale.y);
	node["globalPositionz"] = std::to_string(originalScale.z);

	node["localMatrixx"] = std::to_string(localMatrix.x);
	node["localMatrixy"] = std::to_string(localMatrix.y);
	node["localMatrixz"] = std::to_string(localMatrix.z);
	node["localMatrixw"] = std::to_string(localMatrix.w);

	node["globalMatrixx"] = std::to_string(globalMatrix.x);
	node["globalMatrixy"] = std::to_string(globalMatrix.y);
	node["globalMatrixz"] = std::to_string(globalMatrix.z);
	node["globalMatrixw"] = std::to_string(globalMatrix.w);

	node["scalex"] = std::to_string(scale.x);
	node["scaley"] = std::to_string(scale.y);
	node["scalez"] = std::to_string(scale.z);

	node["radius"] = std::to_string(radius);

	node["height"] = std::to_string(height);

	node["lastIndex"] = std::to_string(lastIndex);

	node["colliderType"] = std::to_string(colliderType);

	node["tmpScalex"] = std::to_string(tmpScale.x);
	node["tmpScaley"] = std::to_string(tmpScale.y);
	node["tmpScalez"] = std::to_string(tmpScale.z);

	node["firstCreation"] = std::to_string(firstCreation);

	return node;
}

void ComponentCollider::Load(json& node)
{
	ENGINE_CONSOLE_LOG("Load");
	CreateCollider(COLLIDER_TYPE::NONE, true);

	std::string localPositionx = node["localPositionx"];
	std::string localPositiony = node["localPositiony"];
	std::string localPositionz = node["localPositionz"];

	std::string originalScalex = node["originalScalex"];
	std::string originalScaley = node["originalScaley"];
	std::string originalScalez = node["originalScalez"];

	std::string offsetx = node["offsetx"];
	std::string offsety = node["offsety"];
	std::string offsetz = node["offsetz"];

	std::string localMatrixx = node["localMatrixx"];
	std::string localMatrixy = node["localMatrixy"];
	std::string localMatrixz = node["localMatrixz"];
	std::string localMatrixw = node["localMatrixw"];

	std::string globalMatrixx = node["globalMatrixx"];
	std::string globalMatrixy = node["globalMatrixy"];
	std::string globalMatrixz = node["globalMatrixz"];
	std::string globalMatrixw = node["globalMatrixw"];

	std::string scalex = node["scalex"];
	std::string scaley = node["scaley"];
	std::string scalez = node["scalez"];

	std::string radius_ = node["radius"];

	std::string height_ = node["height"];

	std::string lastIndex_ = node["lastIndex"];

	std::string colliderType_ = node["colliderType"];

	std::string tmpScalex = node["tmpScalex"];
	std::string tmpScaley = node["tmpScaley"];
	std::string tmpScalez = node["tmpScalez"];

	std::string firstCreation_ = node["firstCreation"];

	localPosition = float3(std::stof(localPositionx), std::stof(localPositiony), std::stof(localPositionz));
	originalScale = float3(std::stof(originalScalex), std::stof(originalScaley), std::stof(originalScalez));
	offset = float3(std::stof(offsetx), std::stof(offsety), std::stof(offsetz));
	
	localMatrix.x = std::stof(localMatrixx);
	localMatrix.y = std::stof(localMatrixy);
	localMatrix.z = std::stof(localMatrixz);
	localMatrix.w = std::stof(localMatrixw);

	globalMatrix.x = std::stof(globalMatrixx);
	globalMatrix.y = std::stof(globalMatrixy);
	globalMatrix.z = std::stof(globalMatrixz);
	globalMatrix.w = std::stof(globalMatrixw);

	scale = float3(std::stof(scalex), std::stof(scaley), std::stof(scalez));

	radius = std::stof(radius_);
	height = std::stof(height_);
	lastIndex = std::stoi(lastIndex_);
	colliderType = std::stoi(colliderType_);

	tmpScale = float3(std::stof(tmpScalex), std::stof(tmpScaley), std::stof(tmpScalez));

	firstCreation = true;

	toPlay = false;

	std::string colliderEnum = node["colliderType"];

	lastIndex = 0;

	int tmp = std::stoi(colliderEnum);
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
	if (ImGui::TreeNodeEx("Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Combo("Type", &colliderType, "NONE\0BOX\0SPHERE\0CAPSULE\0\0");

		switch (colliderType)
		{
		case 0:
			type = ComponentCollider::COLLIDER_TYPE::NONE;
			CreateCollider(type);
			break;
		case 1:
			type = ComponentCollider::COLLIDER_TYPE::BOX;
			CreateCollider(type);
			break;
		case 2:
			type = ComponentCollider::COLLIDER_TYPE::SPHERE;
			CreateCollider(type);
			break;
		case 3:
			type = ComponentCollider::COLLIDER_TYPE::CAPSULE;
			CreateCollider(type);
			break;
		}

		if (shape)
		{
			float3* position = &localPosition;
			
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
				case PxGeometryType::eSPHERE:
				{
					float prevRadius = radius;
					
					ImGui::Text("Radius");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##R", &radius, 0.005f);
					
					scale.x = radius;
					scale.y = radius;
					scale.z = radius;

					if (prevRadius != radius || editCollider)
						CreateCollider(COLLIDER_TYPE::SPHERE, true);
								
					break;
				}

				case PxGeometryType::eBOX:
				{
					float3 prevScale = scale;
					ImGui::Text("Size");
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SX", &scale.x, 0.005f, 0.01f, 1000.0f);

					ImGui::SameLine();

					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SY", &scale.y, 0.005f, 0.01f, 1000.0f);

					ImGui::SameLine();

					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SZ", &scale.z, 0.005f, 0.01f, 1000.0f);

					if (prevScale.x != scale.x || prevScale.y != scale.y || prevScale.z != scale.z || editCollider)
						CreateCollider(COLLIDER_TYPE::BOX, true);

					break;
				}

				case PxGeometryType::eCAPSULE:
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

					scale.x = radius;
					scale.y = height;
					scale.z = radius;

					if (prevRadius != radius || prevheight != height || editCollider)
						CreateCollider(COLLIDER_TYPE::CAPSULE, true);

					break;
				}

			}
		}

		ImGui::TreePop();
	}

}

void ComponentCollider::CreateCollider(ComponentCollider::COLLIDER_TYPE type, bool createAgain) {
	if (shape != nullptr && (lastIndex != (int)type || createAgain)) {
		shape->release();
		shape = nullptr;
		if (GO->GetComponent<ComponentDynamicRigidBody>() != nullptr)
		{
			if (GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody != nullptr)
				App->physics->mScene->removeActor(*(PxActor*)GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody);
			if (createAgain && rigidStatic)
				App->physics->mScene->removeActor(*(PxActor*)rigidStatic);
		}

		else
			App->physics->mScene->removeActor(*(PxActor*)rigidStatic);


		// --- Make sure to always enter here or else the mesh's data won't be released!!! ---
		if (mesh && mesh->IsInMemory())
		{
			mesh->Release();
		}
	}

	if (lastIndex == (int)type && !createAgain) {
		return;
	}

	PxTransform localTransform(PxVec3(localPosition.x, localPosition.y, localPosition.z));
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	float3 tScale = transform->GetScale();

	switch (type) {
		case ComponentCollider::COLLIDER_TYPE::BOX: {
		
			float3 center;

			PxBoxGeometry boxGeometry;// (PxVec3(baseScale.x, baseScale.y, baseScale.z));
			Quat q = transform->rotation;
			Quat q2 = q;
			if (!firstCreation)
			{
				
				q.InverseAndNormalize();
				transform->rotation = q;
				transform->SetGlobalTransform(transform->Local_transform);
				GO->UpdateAABB();

				originalScale = GO->GetOBB().Size();
				center = GO->GetAABB().CenterPoint();

				/*transform->rotation = q2;
				transform->SetGlobalTransform(transform->Local_transform);
				GO->UpdateAABB();*/

				offset = center;//returns the offset of the collider from the AABB
				firstCreation = true;

				//originalScale = GO->GetAABB().HalfSize().Mul(scale) * 2;
				//offset = center - transform->GetGlobalPosition();//returns the offset of the collider from the AABB
				//firstCreation = true; 
			}
			offset.Mul(tScale);

			boxGeometry = PxBoxGeometry(PxVec3(originalScale.x * tScale.x * scale.x * 0.5, originalScale.y * tScale.y * scale.y * 0.5, originalScale.z * tScale.z * scale.z * 0.5));
			
			shape = App->physics->mPhysics->createShape(boxGeometry, *App->physics->mMaterial);
			shape->setGeometry(boxGeometry);
			
			PxTransform position(PxVec3(center.x, center.y, center.z));
			//centerPosition = center;
			
			if (!HasDynamicRigidBody(boxGeometry, position))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, position, *shape);
				App->physics->mScene->addActor(*rigidStatic);
			}

			transform->rotation = q2;
			transform->SetGlobalTransform(transform->Local_transform);
			GO->UpdateAABB();

			UpdateLocalMatrix();

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::BOX;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::SPHERE: {
			PxSphereGeometry SphereGeometry(radius);
			shape = App->physics->mPhysics->createShape(SphereGeometry, *App->physics->mMaterial);
			shape->setGeometry(SphereGeometry);

			if (!HasDynamicRigidBody(SphereGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->mScene->addActor(*rigidStatic);
			}

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::SPHERE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::PLANE: {
			PxBoxGeometry planeGeometry(PxVec3(scale.x, 0.0001f, scale.z));
			shape = App->physics->mPhysics->createShape(planeGeometry, *App->physics->mMaterial);
			shape->setGeometry(planeGeometry);

			if (!HasDynamicRigidBody(planeGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->mScene->addActor(*rigidStatic);
			}

			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::PLANE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::CAPSULE: {
			PxCapsuleGeometry CapsuleGeometry(radius, height);
			shape = App->physics->mPhysics->createShape(CapsuleGeometry, *App->physics->mMaterial);
			shape->setGeometry(CapsuleGeometry);

			if (!HasDynamicRigidBody(CapsuleGeometry, localTransform))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, localTransform, *shape);
				App->physics->mScene->addActor(*rigidStatic);
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
			App->physics->mScene->removeActor(*(PxActor*)GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody);
		if (rigidStatic)
			App->physics->mScene->removeActor(*(PxActor*)rigidStatic);
	}

	else
		App->physics->mScene->removeActor(*(PxActor*)rigidStatic);
}


template <class Geometry>
bool ComponentCollider::HasDynamicRigidBody(Geometry geometry, PxTransform transform) const
{
	ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();
	
	if (dynamicRB != nullptr)
	{
		dynamicRB->rigidBody = PxCreateDynamic(*App->physics->mPhysics, transform, geometry, *App->physics->mMaterial, 1.0f);
		App->physics->mScene->addActor(*dynamicRB->rigidBody);

		return true;
	}

	else
		return false;
}