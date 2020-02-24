#include "ComponentCollider.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModulePhysics.h"
#include "ModuleSceneManager.h"

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

void ComponentCollider::Draw() const
{
	if (shape)
		{
			// --- Get shape's dimensions ---
			PxGeometryHolder holder = shape->getGeometry();
			PxGeometryType::Enum type = holder.getType();

			// --- Draw shape according to type ---
			switch (type)
			{
			case physx::PxGeometryType::eSPHERE: {
				PxSphereGeometry shape = holder.sphere();
				PxReal radiusSphere = shape.radius;

				//Create SPHERE

				break;
			}
			case physx::PxGeometryType::ePLANE:
				break;
			case physx::PxGeometryType::eCAPSULE: {

				PxCapsuleGeometry capsule = holder.capsule();
				PxReal radius = capsule.radius;
				PxReal halfHeight = capsule.halfHeight;

				//Create CAPSULE

				break;
			}
			case physx::PxGeometryType::eBOX: {
				PxBoxGeometry box = holder.box();
				PxVec3 length = 2 * box.halfExtents;

				AABB aabb;
				aabb.SetFromCenterAndSize(vec(globalPosition.p.x, globalPosition.p.y, globalPosition.p.z), vec(length.x + 20, length.y, length.z));

				ModuleSceneManager::DrawWire(aabb, Red, App->scene_manager->GetPointLineVAO());
				break;
			}
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

void ComponentCollider::SetPosition()
{
	PxTransform localTransform(PxVec3(localPosition.x, localPosition.y, localPosition.z));
	shape->setLocalPose(localTransform);
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
		ImGui::Combo("Type", &colliderType, "NONE\0BOX\0SPHERE\0\0");

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
		}


		//if (ImGui::Checkbox("Edit Collider", &collider->editCollider))
		//{
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
		//}

		ImGui::TreePop();
	}

}

void ComponentCollider::CreateCollider(ComponentCollider::COLLIDER_TYPE type) {
	if (shape != nullptr && lastIndex != (int)type)
		shape->release();

	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();
	globalPosition = PxTransform(PxVec3(pos.x, pos.y, pos.z));

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
