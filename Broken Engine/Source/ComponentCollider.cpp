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
	ComponentTransform* transform = ContainerGO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	globalPosition = PxTransform(PxVec3(pos.x, pos.y, pos.z));
	PxBoxGeometry geometry(PxVec3(0.5f, 0.5f, 0.5f));
	box = PxCreateDynamic(*App->physics->mPhysics, globalPosition, geometry, *App->physics->mMaterial, 1.0f);

	App->physics->mScene->addActor(*box);
}

ComponentCollider::~ComponentCollider()
{
}

void ComponentCollider::Draw() const
{
	if (box)
	{
		// --- Retrieve number of shapes associated to rigid dynamic body ---
		uint shapes = box->getNbShapes();

		if (shapes > 0)
		{
			// --- Retrieve pointer to shape, for now we only care about the first one ---
			PxShape** shape = new PxShape*[1];
			uint written = box->getShapes(shape, sizeof(PxShape*), shapes - 1);

			if (shape)
			{
				// --- Get shape's dimensions ---
				PxGeometryHolder holder = (*shape)->getGeometry();
				PxGeometryType::Enum type = holder.getType();

				// --- Draw shape according to type ---
				switch (type)
				{
				case physx::PxGeometryType::eSPHERE:
					break;
				case physx::PxGeometryType::ePLANE:
					break;
				case physx::PxGeometryType::eCAPSULE:
					break;
				case physx::PxGeometryType::eBOX:
				{
					PxBoxGeometry box = holder.box();
					PxVec3 dimensions = 2 * box.halfExtents;

					// --- Use data to create an AABB and draw it ---
					AABB aabb;
					aabb.SetFromCenterAndSize(vec(globalPosition.p.x, globalPosition.p.y, globalPosition.p.z), vec(dimensions.x, dimensions.y, dimensions.z));

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

			// --- Delete shape pointer array ---
			delete[] shape;
		}
	}
}

void ComponentCollider::SetPosition()
{
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	float3 position(pos.x * localPosition.x, pos.y * localPosition.y, pos.z * localPosition.z);
	PxTransform globalTransform(PxVec3(position.x, position.y, position.z));
	PxTransform transtest;
	box->setGlobalPose(globalTransform);
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
			break;
		case 2:
			collider->type = ComponentCollider::COLLIDER_TYPE::SPHERE;
			break;
		}

		if (ImGui::Checkbox("Edit Collider", &collider->editCollider))
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
		}

		ImGui::TreePop();
	}
}
 