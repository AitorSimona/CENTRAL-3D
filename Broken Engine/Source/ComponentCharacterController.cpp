#include "ComponentCharacterController.h"
#include "Application.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceShader.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ComponentTransform.h"

#include "PhysX_3.4/Include/characterkinematic/PxController.h"
#include "PhysX_3.4/Include/characterkinematic/PxCapsuleController.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ComponentCharacterController::ComponentCharacterController(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::CharacterController)
{
	float3 pos = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();
	capsuleDesc.position = physx::PxExtendedVec3(pos.x, pos.y, pos.z);
	capsuleDesc.contactOffset = contactOffset;
	capsuleDesc.stepOffset = stepOffset;
	capsuleDesc.slopeLimit = slopeLimit;
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	capsuleDesc.upDirection = physx::PxVec3(0, 1, 0);
	capsuleDesc.material = App->physics->mMaterial;

	desc = &capsuleDesc;

	controller = App->physics->mControllerManager->createController(*desc);
	App->physics->mScene->addActor(*controller->getActor());

	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCharacterController");
}

ComponentCharacterController::~ComponentCharacterController()
{
	mesh->Release();
}

void ComponentCharacterController::Update()
{
	// BORRAR MESH AL TOCAR EL INSPECTOR
}

void ComponentCharacterController::Draw()
{
	if (!mesh->IsInMemory())
	{
		// --- Rebuild capsule ---
		App->scene_manager->CreateCapsule(radius, height, mesh);
		mesh->LoadToMemory();
	}

	// --- Render shape ---
	if (mesh && mesh->IsInMemory() && mesh->vertices && mesh->Indices)
	{
		// --- Use default shader ---
		glUseProgram(App->renderer3D->defaultShader->ID);

		// --- Set uniforms ---
		GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, GO->GetComponent<ComponentTransform>()->GetGlobalTransform().Transposed().ptr());

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

json ComponentCharacterController::Save() const
{
	return json();
}

void ComponentCharacterController::Load(json& node)
{
}

void ComponentCharacterController::CreateInspectorNode()
{
	if (ImGui::TreeNodeEx("Character Controller", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat("Radius", &radius, 0.005f))
		{
			if (mesh && mesh->IsInMemory())
				mesh->Release();
			
			SetRadius(radius);
		}

		if (ImGui::DragFloat("Height", &height, 0.005f))
		{
			if (mesh && mesh->IsInMemory())
				mesh->Release();	

			SetHeight(height);
		}

		if (ImGui::DragFloat("contactOffset", &contactOffset, 0.005f))
		{
			if (mesh && mesh->IsInMemory())
				mesh->Release();

			SetContactOffset(contactOffset);
		}

		if (ImGui::DragFloat("stepOffset", &stepOffset, 0.005f))
		{
			if (mesh && mesh->IsInMemory())
				mesh->Release();

			SetStepOffset(stepOffset);
		}

		if (ImGui::DragFloat("slopeLimit", &slopeLimit, 0.005f))
		{
			if (mesh && mesh->IsInMemory())
				mesh->Release();

			SetSlopeLimit(slopeLimit);
		}

		ImGui::TreePop();
	}
}

void ComponentCharacterController::SetContactOffset(float offset)
{
	controller->setContactOffset(offset);
}

void ComponentCharacterController::SetStepOffset(float offset)
{
	controller->setStepOffset(offset);
}

void ComponentCharacterController::SetSlopeLimit(float limit)
{
	controller->setSlopeLimit(limit);
}

void ComponentCharacterController::SetRadius(float radius)
{
	capsuleDesc.radius = radius;
}

void ComponentCharacterController::SetHeight(float height)
{
	capsuleDesc.height = height;
}
