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
	ComponentTransform* transform = ContainerGO->GetComponent<ComponentTransform>();
	float3 pos = transform->GetPosition();

	globalPosition = PxTransform(PxVec3(pos.x, pos.y, pos.z));
	PxBoxGeometry pxbox(PxVec3(0.5f, 0.5f, 0.5f));
	PxCapsuleGeometry pxcapsule(1.0f, 1.0f);
	PxSphereGeometry pxsphere(1.0f);

	// Plane does not work
	//PxPlaneGeometry pxplane;

	box = PxCreateDynamic(*App->physics->mPhysics, globalPosition, pxsphere, *App->physics->mMaterial, 1.0f);

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCollider");

	App->physics->mScene->addActor(*box);
}

ComponentCollider::~ComponentCollider()
{
	if (mesh)
	{
		Resource* res = mesh;
		res->OnDelete();
		delete mesh;
	}
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
				{
					PxSphereGeometry pxsphere = holder.sphere();

					// --- Only creating sphere once for now ---
					if (!mesh->IsInMemory())
					{
						// --- If the sphere has to be modified, call mesh.Release() first
						App->scene_manager->CreateSphere(pxsphere.radius, 25, 25, mesh);
						mesh->LoadToMemory();
					}
				}
					break;
				case physx::PxGeometryType::ePLANE:
				{
					PxPlaneGeometry pxplane = holder.plane();

					// --- Only creating sphere once for now ---
					if (!mesh->IsInMemory())
					{
						// --- If the sphere has to be modified, call mesh.Release() first
						App->scene_manager->CreatePlane(10,10,10,mesh);
						mesh->LoadToMemory();
					}
				}
					break;
				case physx::PxGeometryType::eCAPSULE:
				{
					PxCapsuleGeometry pxcapsule = holder.capsule();
					uint height = pxcapsule.halfHeight;
					uint radius = pxcapsule.radius;

					// --- Only creating capsule once for now ---
					if (!mesh->IsInMemory())
					{
						// --- If the capsule has to be modified, call mesh.Release() first
						App->scene_manager->CreateCapsule(radius, height, mesh);
						mesh->LoadToMemory();
					}
				
				}
					break;
				case physx::PxGeometryType::eBOX:
				{
					PxBoxGeometry pxbox = holder.box();
					PxVec3 dimensions = 2 * pxbox.halfExtents;

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

		// --- Render shape ---
		if (mesh->vertices && mesh->Indices)
		{
			// --- Use default shader ---
			glUseProgram(App->renderer3D->defaultShader->ID);

			ComponentTransform* transform = GO->GetComponent<ComponentTransform>();

			// --- Set uniforms ---
			GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobalTransform().Transposed().ptr());

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
 