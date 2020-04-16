#include "GameObject.h"
#include "Application.h"
#include "GameObject.h"
#include "Components.h"

#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModulePhysics.h"
#include "ComponentAudioListener.h"
#include "ComponentAudioSource.h"
#include "ComponentCanvas.h"
#include "ComponentText.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "ComponentCharacterController.h"
#include "ComponentCollider.h"

//#include "ComponentCheckBox.h"
//#include "ComponentInputText.h"
#include "ComponentProgressBar.h"
#include "ComponentCircularBar.h"
#include "ComponentLight.h"

#include "ResourceModel.h"
#include "ResourceScene.h"

#include "mmgr/mmgr.h"

using namespace Broken;

GameObject::GameObject(const char* name) : name(name)
{
	UID = App->GetRandom().Int();
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();
	layer = LAYER_0;

	collisions.resize(6, nullptr);

	Enable();
}

GameObject::GameObject(const char* name, uint UID) : name(name), UID(UID)
{
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();

	collisions.resize(6, nullptr);

	Enable();
}

GameObject::~GameObject()
{
	// --- Destroy all components and game object ---

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		if (*it)
			delete* it;

	}
	components.clear();

	if (Static)
		App->scene_manager->update_tree = true;

	if (model)
		model->Release();
}

void GameObject::Update(float dt)
{
	// PHYSICS: TEMPORAL example, after updating transform, update collider
	ComponentCollider* collider = GetComponent<ComponentCollider>();

	if (collider)
		collider->UpdateLocalMatrix();

	if (GetComponent<ComponentTransform>()->update_transform)
		TransformGlobal();

	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->GetActive())
			components[i]->Update();
	}

	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

void GameObject::Draw()
{
	if (App->renderer3D->display_boundingboxes)
		App->renderer3D->DrawAABB(GetAABB(), Green);

	// --- Call components Draw ---
	for (int i = 0; i < components.size(); ++i)
	{
		if(components[i] && components[i]->GetActive())
			components[i]->DrawComponent();
	}

}

void GameObject::RecursiveDelete()
{
	// --- Delete all childs of given GO ---

	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			(*it)->RecursiveDelete();
			delete *it;
		}

		childs.clear();
	}

	std::unordered_map<uint, GameObject*>::iterator it;

	// --- If go is static eliminate it from octree and scene static go map ---
	if (Static)
	{
		it = App->scene_manager->currentScene->StaticGameObjects.find(UID);

		if (it != App->scene_manager->currentScene->StaticGameObjects.end())
			App->scene_manager->currentScene->StaticGameObjects.erase(UID);

		App->scene_manager->tree.Erase(this);
	}
	else // If it is not static just eliminate it from scene nostatic go map ---
	{
		it = App->scene_manager->currentScene->NoStaticGameObjects.find(UID);;

		if (it != App->scene_manager->currentScene->NoStaticGameObjects.end())
			App->scene_manager->currentScene->NoStaticGameObjects.erase(UID);
	}
}

//void GameObject::OnUpdateTransform()
//{
//	if (Static)
//		return;
//
//	ComponentTransform* transform = GetComponent<ComponentTransform>();
//
//	if(parent)
//		transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>()->GetGlobalTransform());
//
//	ComponentCamera* camera = GetComponent<ComponentCamera>();
//
//	if(camera)
//		camera->OnUpdateTransform(transform->GetGlobalTransform());
//
//	// --- Update all children ---
//	if (childs.size() > 0)
//	{
//		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
//		{
//			(*it)->OnUpdateTransform();
//		}
//	}
//
//	UpdateAABB();
//}

//Why is there this GameObject* GO if the object passed is the object itself (this)?
void GameObject::TransformGlobal()
{
	ComponentTransform* transform = GetComponent<ComponentTransform>();
	float4x4 old_transform = transform->GetGlobalTransform();

	if (parent)
	transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>()->GetGlobalTransform());

	ComponentCamera* camera = GetComponent<ComponentCamera>();

	if (camera)
	camera->OnUpdateTransform(transform->GetGlobalTransform());

	for (std::vector<GameObject*>::iterator tmp = childs.begin(); tmp != childs.end(); ++tmp)
	{
		(*tmp)->TransformGlobal();
	}

	if (is_been_reparented)
	{
		transform->SetGlobalTransform(old_transform);
		is_been_reparented = false;
	}
	
}

void GameObject::RemoveChildGO(GameObject* GO)
{
	if (GO == nullptr)
		return;

	// --- Remove given child from list ---
	if (GO->index >= 0 && GO->index < childs.size() && 
		childs[GO->index] != nullptr && GO->GetUID() == childs[GO->index]->GetUID()) {
		childs.erase(childs.begin() + GO->index);

		// --- We update the indices of the rest of childs ---
		for (int i = GO->index; i < childs.size(); ++i)
			if (childs[i] != nullptr) childs[i]->index -= 1;

		GO->index = -1;
	}
	//else {
	//	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go) {
	//		if (*go == nullptr)
	//			continue;

	//		if ((*go)->GetUID() == GO->GetUID()) {
	//			go = childs.erase(go);
	//			while (go != childs.end()) {
	//				if (*go != nullptr) (*go)->index -= 1;
	//				go++;
	//			}

	//			break;
	//		}
	//	}
	//}
}

Component* GameObject::GetComponentWithUID(uint UUID)
{
	for (int i = 0; i < this->components.size(); ++i) {
		if (this->components[i]->GetUID() == UUID)
			return this->components[i];
	}

	return nullptr;
}

void GameObject::AddChildGO(GameObject* GO, int index) {
	// --- Add a child GO to a Game Object this ---
	if (!FindChildGO(GO)) {

		// --- If it has a parent we remove it from its parent --
		if (GO->parent != nullptr)
			GO->parent->RemoveChildGO(GO);

		GO->parent = this;

		// --- If index was specified, insert ---
		if (index >= 0) {
			// --- Reserve needed space, note that we may leave empty spaces!!! ---
			if (index + 1 > childs.size())
				childs.resize(index + 1);


			// --- Delete element at given index ---
			if (childs[index]) {
				delete childs[index];
				childs[index] = nullptr;
			}

			// --- Insert element at given index ---
			childs[index] = GO;
			GO->index = index;
		}
		// --- Else push back ---
		else {
			childs.push_back(GO);
			GO->index = childs.size() - 1;
		}
	}
}

void GameObject::InsertChildGO(GameObject* GO, int index) {
	if (!FindChildGO(GO)) {
		if (index >= 0 && index < childs.size()) {
			// --- If it has a parent we remove it from its parent --
			if (GO->parent)
				GO->parent->RemoveChildGO(GO);

			GO->parent = this;

			// --- We accomodate for the new size ---
			childs.resize(childs.size() + 1);

			// --- We move all the GOs from index one position up ---
			for (int i = childs.size() - 1; i > index; --i) {
				childs[i] = childs[i - 1];
				childs[i]->index = i;
			}

			// --- We insert the new child ---
			childs[index] = GO;
			GO->index = index;
		}

	}
}

bool GameObject::FindChildGO(GameObject* GO)
{
	// --- Look for given GO in child list and return true if found ---
	bool ret = false;

	if (GO != nullptr) {
		/*for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go) {
			if (*go == nullptr)
				continue;
			else if ((*go)->GetUID() == GO->GetUID())
				ret = true;
		}*/
		if (GO->index >= 0 && GO->index < childs.size() && childs[GO->index] != nullptr) {
			ret = GO->GetUID() == childs[GO->index]->GetUID();
		}
	}

	return ret;
}

bool GameObject::FindParentGO(GameObject* GO)
{
	if (parent == nullptr) return false;
	if (parent->GetUID() == GO->GetUID()) return true;

	return parent->FindParentGO(GO);
}

void GameObject::GetAllChilds(std::vector<GameObject*>& collector)
{
	collector.push_back(this);
	for (uint i = 0; i < childs.size(); i++)
		childs[i]->GetAllChilds(collector);
}

GameObject* GameObject::GetAnimGO(GameObject* GO)
{
	ComponentAnimation* anim = GO->GetComponent<ComponentAnimation>();

	if (anim != nullptr)
	{
		return anim->GetContainerGameObject();
	}
	else
	{
		if (GO->parent)
			return GetAnimGO(GO->parent);
		else
			return nullptr;
	}

}

Component * GameObject::AddComponent(Component::ComponentType type, int index)
{
	BROKEN_ASSERT(static_cast<int>(Component::ComponentType::Unknown) == 21, "Component Creation Switch needs to be updated");
	Component* component = nullptr;

	// --- Check if there is already a component of the type given --- & if it can be repeated
	bool repeatable_component = false;
	std::vector<int>::iterator it = App->scene_manager->repeatable_components.begin();
	for (; it != App->scene_manager->repeatable_components.end(); ++it)
	{
		if ((int)type == (*it))
			repeatable_component = true;
	}

	if (HasComponent(type) == nullptr || repeatable_component == true)
	{
		switch (type)
		{
		case Component::ComponentType::Transform:
			component = new ComponentTransform(this);
			break;
		case Component::ComponentType::Mesh:
			component = new ComponentMesh(this);
			UpdateAABB();
			break;
		case Component::ComponentType::MeshRenderer:
			component = new ComponentMeshRenderer(this);
			break;
		case Component::ComponentType::Camera:
			component = new ComponentCamera(this);
			break;
		case Component::ComponentType::Collider:
			component = new ComponentCollider(this);
			break;
		case Component::ComponentType::DynamicRigidBody:
			component = new ComponentDynamicRigidBody(this);
			break;
		case Component::ComponentType::ParticleEmitter:
			component = new ComponentParticleEmitter(this);
			break;
		case Component::ComponentType::AudioSource:
			component = new ComponentAudioSource(this);
			break;
		case Component::ComponentType::AudioListener:
			component = new ComponentAudioListener(this);
			break;
		case Component::ComponentType::Bone:
			component = new ComponentBone(this);
			break;
		case Component::ComponentType::Animation:
			component = new ComponentAnimation(this);
			break;

		case Component::ComponentType::Canvas:
			component = new ComponentCanvas(this);
			break;

		case Component::ComponentType::Text:
			component = new ComponentText(this);
			break;

		case Component::ComponentType::Image:
			component = new ComponentImage(this);
			break;

		case Component::ComponentType::Script:
			component = new ComponentScript(this);
			break;

		case Component::ComponentType::Button:
			component = new ComponentButton(this);
			break;

		case Component::ComponentType::CharacterController:
			component = new ComponentCharacterController(this);
			break;

		//case Component::ComponentType::CheckBox:
		//	component = new ComponentCheckBox(this);
		//	break;

		//case Component::ComponentType::InputText:
		//	component = new ComponentInputText(this);
		//	break;

		case Component::ComponentType::ProgressBar:
			component = new ComponentProgressBar(this);
			break;

		case Component::ComponentType::CircularBar:
			component = new ComponentCircularBar(this);
			break;

		//Lights
		case Component::ComponentType::Light:
			component = new ComponentLight(this);
			break;
		}

		if (component)
		{
			// --- If index was specified, insert ---
			if (index >= 0)
			{
				// --- Reserve needed space, note that we may leave empty spaces!!! ---
				if(index+1 > components.size())
												components.resize(index+1);


				// --- Delete element at given index ---
				if (components[index])
				{
					delete components[index];
					components[index] = nullptr;
				}

				// --- Insert element at given index ---
				components[index] = component;
			}
			// --- Else push back ---
			else
				components.push_back(component);

		}

	}
	else {
		// --- If we find a component of the same type, tell the user and return such component ---
		ENGINE_CONSOLE_LOG("![Warning]: The current Game Object already has a component of the type given");
		component = HasComponent(type);
	}

	return component;
}

void GameObject::RemoveComponent(Component* comp) {
	// ---Remove component specified component of game object ---

	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if ((*it) && (*it) == comp)
		{
			delete (*it);
			components.erase(it);
			break;
		}
	}
}

Component* GameObject::HasComponent(Component::ComponentType type) const {
	// --- Search for given type of component ---
	Component* component = nullptr;

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->GetType() == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}

std::vector<Component*>& GameObject::GetComponents() {
	return components;
}

void GameObject::Enable() {
	active = true;

	for (int i = 0; i < components.size(); ++i)
		components[i]->Enable();

	for (int i = 0; i < childs.size(); ++i)
		childs[i]->Enable();

}

void GameObject::Disable() {
	active = false;

	for (int i = 0; i < components.size(); ++i)
		components[i]->Disable();

	for (int i = 0; i < childs.size(); ++i)
		childs[i]->Disable();
}

uint& GameObject::GetUID()
{
	return UID;
}

void GameObject::SetUID(uint uid)
{
	// --- Try to eliminate go from current scene ---
	App->scene_manager->currentScene->NoStaticGameObjects.erase(UID);
	App->scene_manager->currentScene->StaticGameObjects.erase(UID);

	UID = uid;

	if (Static)
	{
		App->scene_manager->currentScene->StaticGameObjects[UID] = this;
	}
	else
	{
		App->scene_manager->currentScene->NoStaticGameObjects[UID] = this;

	}
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

const AABB& GameObject::GetAABB()
{
	UpdateAABB();
	return aabb;
}

const OBB& GameObject::GetOBB() const
{
	return obb;
}

int GameObject::GetChildGOIndex(GameObject* GO)
{
	// --- Look for given GO in child list and return its index ---
	int ret = -1;
	uint i = 0; 

	if (childs.size() > 0) 
	{
		for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go) 
		{
			if ((*go)->GetUID() == GO->GetUID())
			{
				ret = i;
			}

			i++;
		}
	}

	return ret;
}

bool& GameObject::GetActive()
{
	return active;
}

int GameObject::GetLayer(){
	return layer;
}

bool GameObject::IsEnabled() const {
	return active;
}

void GameObject::SetName(const char* name)
{
	if (name && name != "root")
		this->name = name;
}

void GameObject::UpdateAABB() {
	ComponentMesh* mesh = GetComponent<ComponentMesh>();
	ComponentTransform* transform = GetComponent<ComponentTransform>();

	if (mesh) {
		obb = mesh->GetAABB();
		obb.Transform(transform->GetGlobalTransform());
		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	if (!mesh)
	{
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(1, 1, 1));
		obb = aabb;
	}
}

void GameObject::ONResourceEvent(uint uid, Resource::ResourceNotificationType type) {
	for (uint i = 0; i < components.size(); ++i) {
		components[i]->ONResourceEvent(uid, type);
	}

	// MYTODO: Remove this
	if (model && type == Resource::ResourceNotificationType::Deletion && model->GetUID() == uid)
		model = nullptr;
}
