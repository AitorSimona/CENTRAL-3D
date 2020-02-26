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

			UpdateLocalMatrix();

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, globalMatrix.Transposed().ptr());

			int vertexColorLocation = glGetAttribLocation(App->renderer3D->defaultShader->ID, "color");
			glVertexAttrib3f(vertexColorLocation, 125, 125, 0);

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
		}
	
}

void ComponentCollider::UpdateLocalMatrix() {

	if (!rigidStatic)
		return;

	//Render
	localMatrix.x = localPosition.x + offset.x;
	localMatrix.y = localPosition.y + offset.y;
	localMatrix.z = localPosition.z + offset.z;
	localMatrix.scaleX = scale.x + originalScale.x - 1;
	localMatrix.scaleY = scale.y + originalScale.y - 1;
	localMatrix.scaleZ = scale.z + originalScale.z - 1;

	ComponentTransform* cTransform = GO->GetComponent<ComponentTransform>();
	globalMatrix = cTransform->GetGlobalTransform() * localMatrix;

	//PHYSX DEBUG
	float3 pos, scale;
	Quat rot;
	globalMatrix.Decompose(pos, rot, scale);

	PxVec3 posi(pos.x, pos.y, pos.z);
	PxQuat quati(rot.x, rot.y, rot.z, rot.w);
	PxTransform transform(posi,quati);

	rigidStatic->setGlobalPose(transform);
}

void ComponentCollider::SetPosition()
{
	/*float3 pos = GO->GetComponent<ComponentTransform>()->GetPosition();
	float3 rot = GO->GetComponent<ComponentTransform>()->GetRotation();
	Quat q = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);

	globalPos = PxVec3(pos.x, pos.y, pos.z) + PxVec3(localPosition.x, localPosition.y, localPosition.z);
	PxQuat globalRot = PxQuat(q.x, q.y, q.z, q.w);

	PxTransform globalTransform(globalPos, globalRot);

	shape->setLocalPose(globalTransform);*/

	/*if (!GO->GetComponent<ComponentDynamicRigidBody>())
		rigidStatic->setGlobalPose(globalTransform);
	else
	{
		if (ImGuizmo::IsUsing()) {
			ComponentDynamicRigidBody* dynamicRB = GO->GetComponent<ComponentDynamicRigidBody>();
			dynamicRB->rigidBody->setGlobalPose(globalTransform);
		}
		else {
			if (GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody != nullptr)
			{
				PxTransform transform = GO->GetComponent<ComponentDynamicRigidBody>()->rigidBody->getGlobalPose();
				GO->GetComponent<ComponentTransform>()->SetPosition(transform.p.x, transform.p.y, transform.p.z);
				GO->GetComponent<ComponentTransform>()->SetRotation(Quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
			}
		}		
	}*/
}

json ComponentCollider::Save() const
{
	return json();
}

void ComponentCollider::Load(json& node)
{
}

void ComponentCollider::CreateInspectorNode()
{
	if (ImGui::TreeNode("Collider"))
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
;
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

					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SX", &scale.x, 0.005f);

					ImGui::SameLine();

					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SY", &scale.y, 0.005f);

					ImGui::SameLine();

					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

					ImGui::DragFloat("##SZ", &scale.z, 0.005f);

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
			if (createAgain)
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
		
			float3 center = GO->GetAABB().CenterPoint();
			offset = center - transform->GetGlobalPosition();//returns the offset of the collider from the AABB

			float3 halfSize = GO->GetAABB().HalfSize().Mul(scale);
			PxBoxGeometry boxGeometry;// (PxVec3(baseScale.x, baseScale.y, baseScale.z));

			if (!firstCreation)
			{
				originalScale = halfSize * 2;
				firstCreation = true;
			}
			
			baseScale = halfSize * 2;
			boxGeometry = PxVec3(baseScale.x + tScale.x, baseScale.y + tScale.y, baseScale.z + tScale.z);
			
			shape = App->physics->mPhysics->createShape(boxGeometry, *App->physics->mMaterial);
			shape->setGeometry(boxGeometry);
			
			PxTransform position(PxVec3(center.x, center.y, center.z));
			//centerPosition = center;
			
			if (!HasDynamicRigidBody(boxGeometry, position))
			{
				rigidStatic = PxCreateStatic(*App->physics->mPhysics, position, *shape);
				App->physics->mScene->addActor(*rigidStatic);
			}

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
	}
	editCollider = false;
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