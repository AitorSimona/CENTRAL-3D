#include "ComponentCharacterController.h"
#include "Application.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceShader.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleScripting.h"
#include "ComponentTransform.h"
#include "ComponentScript.h"
#include "ModuleTimeManager.h"
#include "ModuleGui.h"
#include "ModuleInput.h"
#include "ScriptVar.h"
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
	capsuleDesc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
	capsuleDesc.stepOffset = stepOffset;
	capsuleDesc.slopeLimit = cosf(DEGTORAD * slopeLimit);
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	capsuleDesc.nonWalkableMode = physx::PxControllerNonWalkableMode::Enum::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	capsuleDesc.upDirection = physx::PxVec3(0, 1, 0);
	capsuleDesc.material = App->physics->mMaterial;
	capsuleDesc.behaviorCallback = this;

	desc = &capsuleDesc;

	controller = App->physics->mControllerManager->createController(*desc);

	physx::PxShape* shape;
	controller->getActor()->getShapes(&shape, 1);
	App->physics->addActor(shape->getActor(), GO);

	initialPosition = capsuleDesc.position;

	name = "Character Controller";
	mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCharacterController");
}

ComponentCharacterController::~ComponentCharacterController()
{
	mesh->Release();
}

void ComponentCharacterController::Update()
{/*
	if (App->input->GetKey(SDL_SCANCODE_UP))
		velocity.z = -10.0f;

	else if (App->input->GetKey(SDL_SCANCODE_DOWN))
		velocity.z = 10.0f;
	else
		velocity.z = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT))
		velocity.x = 10.0f;

	else if (App->input->GetKey(SDL_SCANCODE_LEFT))
		velocity.x = -10.0f;
	else
		velocity.x = 0.0f;*/

	ComponentTransform* cTransform = GO->GetComponent<ComponentTransform>();

	if (cTransform->updateValues || App->gui->isUsingGuizmo)
	{
		float3 pos = cTransform->GetGlobalPosition();
		controller->setFootPosition(physx::PxExtendedVec3(pos.x, pos.y, pos.z));
	}

	//Move(velocity.x, velocity.z);

	physx::PxExtendedVec3 cctPosition = controller->getFootPosition();
	float3 cctPos(cctPosition.x, cctPosition.y, cctPosition.z);

	if (!creation)
	{
		float offset = radius + height * 0.5f + contactOffset;
		cctPos.y += offset;
		controller->setFootPosition(physx::PxExtendedVec3(controller->getFootPosition().x, controller->getFootPosition().y + offset, controller->getFootPosition().z));
		creation = true;
	}

	GO->GetComponent<ComponentTransform>()->SetPosition((float3)cctPos);

	if (to_delete)
	{
		Delete();
		this->GetContainerGameObject()->RemoveComponent(this);
	}
}

void ComponentCharacterController::Draw()
{
	if (!mesh->IsInMemory())
	{
		// --- Rebuild capsule ---
		App->scene_manager->CreateCapsule(radius * 2, height * 2, mesh);
		mesh->LoadToMemory();
	}

	// --- Render shape ---
	if (mesh && mesh->IsInMemory() && mesh->vertices && mesh->Indices)
	{
		// --- Use default shader ---
		uint shaderID = App->renderer3D->defaultShader->ID;
		glUseProgram(shaderID);

		// --- Set uniforms ---
		GLint modelLoc = glGetUniformLocation(shaderID, "u_Model");

		float4x4 aux = GO->GetComponent<ComponentTransform>()->GetGlobalTransform();
		
		aux.y += controller->getPosition().y - controller->getFootPosition().y;

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, aux.Transposed().ptr());

		int vertexColorLocation = glGetUniformLocation(shaderID, "u_Color");
		glUniform3f(vertexColorLocation, 125, 125, 125);

		int TextureSupportLocation = glGetUniformLocation(shaderID, "u_UseTextures");
		glUniform1i(TextureSupportLocation, 0);

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

void ComponentCharacterController::DrawComponent()
{
	if (!GetActive())
	{
		controller->getActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
		physx::PxShape* shape;
		controller->getActor()->getShapes(&shape, 1);
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);

		if (!hasBeenDeactivated)
		{
			Delete();
			hasBeenDeactivated = true;
		}
	}

	else
	{
		physx::PxShape* shape;
		controller->getActor()->getShapes(&shape, 1);

		if (hasBeenDeactivated)
		{
			App->physics->addActor(shape->getActor(), GO);
			hasBeenDeactivated = false;
		}

		controller->getActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	}

	Draw();
}

void ComponentCharacterController::Move(float velX, float velZ, float minDist)
{
	physx::PxVec3 vel;
	vel.x = velX;
	vel.y = App->physics->mScene->getGravity().y;
	vel.z = velZ;

	physx::PxFilterData filterData;
	filterData.word0 = App->physics->layer_list.at((int)GO->layer).LayerGroup; // layers that will collide

	physx::PxControllerFilters controllerFilter;
	controllerFilter.mFilterData = &filterData;

	controller->move(vel * App->time->GetGameDt(), minDist, App->time->GetGameDt(), controllerFilter);
}

void ComponentCharacterController::Delete()
{
	physx::PxShape* shape = nullptr;

	if (controller != nullptr)
	{
		controller->getActor()->getShapes(&shape, 1);
		App->physics->DeleteActor(shape->getActor());
		controller->release();
		controller = nullptr;
	}

}

json ComponentCharacterController::Save() const
{
	ENGINE_CONSOLE_LOG("Saved");

	json node;

	node["contactOffset"] = std::to_string(contactOffset);
	node["stepOffset"] = std::to_string(stepOffset);
	node["slopeLimit"] = std::to_string(slopeLimit);
	node["radius"] = std::to_string(radius);
	node["height"] = std::to_string(height);
	node["positionX"] = std::to_string(controller->getPosition().x);
	node["positionY"] = std::to_string(controller->getPosition().y);
	node["positionZ"] = std::to_string(controller->getPosition().z);

	if (controller->getNonWalkableMode() == physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING)
		node["nonWalkableMode"] = std::to_string(0);
	else
		node["nonWalkableMode"] = std::to_string(1);

	return node;
}

void ComponentCharacterController::Load(json& node)
{
	ENGINE_CONSOLE_LOG("Load");

	std::string contactOffset_ = node["contactOffset"].is_null() ? "0" : node["contactOffset"];
	std::string stepOffset_ = node["stepOffset"].is_null() ? "0" : node["stepOffset"];
	std::string slopeLimit_ = node["slopeLimit"].is_null() ? "0" : node["slopeLimit"];
	std::string radius_ = node["radius"].is_null() ? "0" : node["radius"];
	std::string height_ = node["height"].is_null() ? "0" : node["height"];
	std::string positionX = node["positionX"].is_null() ? "0" : node["positionX"];
	std::string positionY = node["positionY"].is_null() ? "0" : node["positionY"];
	std::string positionZ = node["positionZ"].is_null() ? "0" : node["positionZ"];
	std::string nonWalkableMode = node["nonWalkableMode"].is_null() ? "0" : node["nonWalkableMode"];
	std::string firstTime_ = node["firstTime"].is_null() ? "0" : node["firstTime"];

	contactOffset = std::stof(contactOffset_);
	stepOffset = std::stof(stepOffset_);
	slopeLimit = std::stof(slopeLimit_);
	radius = std::stof(radius_);
	height = std::stof(height_);
	position.x = std::stof(positionX);
	position.y = std::stof(positionY);
	position.z = std::stof(positionZ);

	SetContactOffset(contactOffset);
	SetStepOffset(stepOffset);
	SetSlopeLimit(slopeLimit);
	SetRadius(radius);
	SetHeight(height);

	if (std::stof(nonWalkableMode) == 0)
	{
		controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::Enum::ePREVENT_CLIMBING);
		sliding = false;
	}
	else
	{
		controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::Enum::ePREVENT_CLIMBING_AND_FORCE_SLIDING);
		sliding = true;
	}

	physx::PxExtendedVec3 pos = physx::PxExtendedVec3(position.x, position.y, position.z);
	float offset = radius + height * 0.5f + contactOffset;
	pos.y -= offset;
	controller->setPosition(pos);
}

void ComponentCharacterController::CreateInspectorNode()
{
	if (ImGui::Button("Delete component"))
		to_delete = true;

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

	ImGui::Text("Non Walkable Mode");
	if (ImGui::Checkbox("##W", &sliding))
	{
		if (sliding)
			controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::Enum::ePREVENT_CLIMBING_AND_FORCE_SLIDING);

		else
			controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::Enum::ePREVENT_CLIMBING);
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
	static_cast<physx::PxCapsuleController*>(controller)->setSlopeLimit(cosf(DEGTORAD * limit));
}

void ComponentCharacterController::SetRadius(float radius)
{
	static_cast<physx::PxCapsuleController*>(controller)->setRadius(radius);
}

void ComponentCharacterController::SetHeight(float height)
{
	static_cast<physx::PxCapsuleController*>(controller)->resize(height);
	//static_cast<physx::PxCapsuleController*>(controller)->setHeight(height);
}

physx::PxControllerBehaviorFlags ComponentCharacterController::getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor)
{

	if (shape.getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)
	{
		GameObject* go = App->physics->actors[(physx::PxRigidActor*) & actor];
		if (go)
		{
			go->collisions.at(Collision_Type::ONTRIGGER_ENTER) = GO;
			GO->collisions.at(Collision_Type::ONTRIGGER_ENTER) = go;

			ComponentScript* script = go->GetComponent<ComponentScript>();
			ComponentScript* script2 = GO->GetComponent<ComponentScript>();
			ScriptFunc function;
			function.name = "OnTriggerEnter";

			if (script)
				App->scripting->CallbackScriptFunction(script, function);
			if (script2)
				App->scripting->CallbackScriptFunction(script2, function);
		}
	}
	else
	{
		GameObject* go = App->physics->actors[(physx::PxRigidActor*) & actor];
		if (go)
		{
			GO->collisions.at(Collision_Type::ONCOLLISION_ENTER) = go;
			go->collisions.at(Collision_Type::ONCOLLISION_ENTER) = GO;

			ComponentScript* script = go->GetComponent<ComponentScript>();
			ComponentScript* script2 = GO->GetComponent<ComponentScript>();

			ScriptFunc function;
			function.name = "OnCollisionEnter";

			if (script)
				App->scripting->CallbackScriptFunction(script, function);
			if (script2)
				App->scripting->CallbackScriptFunction(script2, function);
		}
	}

	return physx::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

physx::PxControllerBehaviorFlags ComponentCharacterController::getBehaviorFlags(const physx::PxController& controller)
{
	physx::PxShape* shape1;
	this->controller->getActor()->getShapes(&shape1, 1);
	physx::PxRigidActor* a = this->controller->getActor();

	physx::PxShape* shape;
	controller.getActor()->getShapes(&shape, 1);
	physx::PxRigidActor* b = controller.getActor();

	if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE || shape1->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)
	{
		GameObject* go = App->physics->actors[shape->getActor()];
		if (go)
		{
			GO->collisions.at(Collision_Type::ONTRIGGER_ENTER) = go;
			go->collisions.at(Collision_Type::ONTRIGGER_ENTER) = GO;

			ComponentScript* script = go->GetComponent<ComponentScript>();
			ComponentScript* script2 = GO->GetComponent<ComponentScript>();
			ScriptFunc function;
			function.name = "OnTriggerEnter";

			if (script)
				App->scripting->CallbackScriptFunction(script, function);
			if (script2)
				App->scripting->CallbackScriptFunction(script2, function);
		}
	}
	else
	{
		GameObject* go = App->physics->actors[shape->getActor()];
		if (go)
		{
			GO->collisions.at(Collision_Type::ONCOLLISION_ENTER) = go;
			go->collisions.at(Collision_Type::ONCOLLISION_ENTER) = GO;

			ComponentScript* script = go->GetComponent<ComponentScript>();
			ComponentScript* script2 = GO->GetComponent<ComponentScript>();

			ScriptFunc function;
			function.name = "OnCollisionEnter";

			if (script)
				App->scripting->CallbackScriptFunction(script, function);
			if (script2)
				App->scripting->CallbackScriptFunction(script2, function);
		}
	}

	return physx::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

physx::PxControllerBehaviorFlags ComponentCharacterController::getBehaviorFlags(const physx::PxObstacle& obstacle)
{
	return physx::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}
