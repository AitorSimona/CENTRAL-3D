#include "ComponentCollider.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModulePhysics.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "OpenGL.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace physx;

ComponentCollider::ComponentCollider(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Collider)
{
	/*ComponentTransform* transform = ContainerGO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	globalPosition = PxTransform(PxVec3(pos.x, pos.y, pos.z));
	PxBoxGeometry geometry(PxVec3(0.5f, 0.5f, 0.5f));
	shape = App->physics->mPhysics->createShape(;
	shape->setGeometry(geometry);

	App->physics->mScene->addActor(*shape->getActor());*/
}

ComponentCollider::~ComponentCollider()
{
}

void ComponentCollider::Draw() 
{
	if (shape)
	{

		if (mesh == nullptr)
		{
			// --- Get shape's dimensions ---
			PxGeometryHolder holder = shape->getGeometry();
			PxGeometryType::Enum type = holder.getType();

			// --- Draw shape according to type ---
			switch (type)
			{
			case physx::PxGeometryType::eSPHERE:
			{
				PxSphereGeometry pxsphere = holder.sphere();
				mesh = (ResourceMesh*)App->resources->GetResource(App->scene_manager->sphere->GetUID());
			}
			break;
			case physx::PxGeometryType::ePLANE:
			{
				PxPlaneGeometry pxplane = holder.plane();
				mesh = (ResourceMesh*)App->resources->GetResource(App->scene_manager->plane->GetUID());
			}
			break;
			case physx::PxGeometryType::eCAPSULE:
			{
				PxCapsuleGeometry capsule = holder.capsule();
				mesh = (ResourceMesh*)App->resources->GetResource(App->scene_manager->capsule->GetUID());
			}
			break;
			case physx::PxGeometryType::eBOX:
			{
				PxBoxGeometry pxbox = holder.box();
				//PxVec3 dimensions = 2 * pxbox.halfExtents;

				// --- Use data to create an AABB and draw it ---
				AABB aabb;
				aabb.SetFromCenterAndSize(vec(globalPosition.x, globalPosition.y, globalPosition.z), vec(scale.x, scale.y, scale.z));

				ModuleSceneManager::DrawWire(aabb, Red, App->scene_manager->GetPointLineVAO());
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

		// --- Render shape ---
		if (mesh && mesh->vertices && mesh->Indices)
		{
			// --- Use default shader ---
			glUseProgram(App->renderer3D->defaultShader->ID);

			ComponentTransform* transform = GO->GetComponent<ComponentTransform>();

			// --- Set uniforms ---
			GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
			
			globalMatrix = transform->GetGlobalTransform() * localMatrix;
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
}

void ComponentCollider::SetPosition()
{
	PxTransform localTransform(PxVec3(localPosition.x, localPosition.y, localPosition.z));
	shape->setLocalPose(localTransform);
	globalPosition = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();
	globalPosition += localPosition;
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
	ComponentCollider* collider = GO->GetComponent<ComponentCollider>();

	if (ImGui::TreeNode("Collider"))
	{
		static int colliderType = 0;
		ImGui::Combo("Type", &colliderType, "NONE\0BOX\0SPHERE\0CAPSULE\0\0");

		switch (colliderType)
		{
		case 0:
			collider->type = ComponentCollider::COLLIDER_TYPE::NONE;
			break;
		case 1:
			collider->type = ComponentCollider::COLLIDER_TYPE::BOX;
			CreateCollider(collider->type);
			break;
		case 2:
			collider->type = ComponentCollider::COLLIDER_TYPE::SPHERE;
			CreateCollider(collider->type);
			break;
		case 3:
			collider->type = ComponentCollider::COLLIDER_TYPE::CAPSULE;
			CreateCollider(collider->type);
			break;
		}


		//if (ImGui::Checkbox("Edit Collider", &collider->editCollider))
		//{
			

			// PUT HERE A SWITCH CASE DEPENDING ON THE COLLIDER DIFFERENT OPTIONS

		if (shape)
		{
			float3* position = &collider->localPosition;
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
					localMatrix.x = position->x;
					localMatrix.y = position->y;
					localMatrix.z = position->z;

					const float3 pos(localMatrix.x, localMatrix.y, localMatrix.z);

					ImGui::Text("Radius");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##R", &radius, 0.005f);
					
					localMatrix.scaleX = radius;
					localMatrix.scaleY = radius;						
					localMatrix.scaleZ = radius;

					break;
				}

				case PxGeometryType::eBOX:
				{
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

					break;
				}

				case PxGeometryType::eCAPSULE:
				{
					localMatrix.x = position->x;
					localMatrix.y = position->y;
					localMatrix.z = position->z;

					const float3 pos(localMatrix.x, localMatrix.y, localMatrix.z);

					ImGui::Text("Radius");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##R", &radius, 0.005f);

					ImGui::Text("Height");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##H", &height, 0.005f);

					localMatrix.scaleX = radius;
					localMatrix.scaleY = height;
					localMatrix.scaleZ = radius;

					break;
				}

			}
			SetPosition();
		}

		//}

		ImGui::TreePop();
	}

}

void ComponentCollider::CreateCollider(ComponentCollider::COLLIDER_TYPE type) {
	if (shape != nullptr && lastIndex != (int)type) {
		shape->release();

		if (mesh)
		{
			mesh->Release();
			mesh = nullptr;
		}
	}

	if (lastIndex == (int)type)
		return;

	switch (type) {
		case ComponentCollider::COLLIDER_TYPE::BOX: {
			PxBoxGeometry boxGeometry(PxVec3(1.0f, 1.0f, 1.0f));
			shape = App->physics->mPhysics->createShape(boxGeometry, *App->physics->mMaterial);
			shape->setGeometry(boxGeometry);
			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::BOX;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::SPHERE: {
			PxSphereGeometry SphereGeometry(1.0f);
			shape = App->physics->mPhysics->createShape(SphereGeometry, *App->physics->mMaterial);
			shape->setGeometry(SphereGeometry);
			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::SPHERE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::PLANE: {
			PxBoxGeometry planeGeometry(PxVec3(1.0f, 0.0001f, 1.0f));
			shape = App->physics->mPhysics->createShape(planeGeometry, *App->physics->mMaterial);
			shape->setGeometry(planeGeometry);
			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::PLANE;
			break;
		}
		case ComponentCollider::COLLIDER_TYPE::CAPSULE: {
			PxCapsuleGeometry CapsukeGeometry(1.0f, 3.0f);
			shape = App->physics->mPhysics->createShape(CapsukeGeometry, *App->physics->mMaterial);
			shape->setGeometry(CapsukeGeometry);
			lastIndex = (int)ComponentCollider::COLLIDER_TYPE::CAPSULE;
			break;
		}
	}
}
