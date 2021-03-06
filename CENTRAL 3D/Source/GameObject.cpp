#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentCamera.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "Math.h"

#include "ResourceModel.h"
#include "ResourceScene.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(const char* name)
{
	UID = App->GetRandom().Int();
	this->name = name;
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();

	Enable();
}

GameObject::GameObject(const char* name, uint UID)
{
	this->UID = UID;
	this->name = name;
	// --- Add transform ---
	AddComponent(Component::ComponentType::Transform);
	UpdateAABB();

	Enable();
}

GameObject::~GameObject()
{
	// --- Destroy all components and game object ---

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
			delete *it;
		
	}
	components.clear();

	if (model)
		model->Release();
}

void GameObject::Update(float dt)
{
	if (GetComponent<ComponentTransform>()->update_transform)
		this->OnUpdateTransform();

	// --- Update components ---
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->GetActive())
			components[i]->Update();
	}

	// --- Update child game objects
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

void GameObject::RecursiveDelete(bool target)
{
	// --- Delete all childs of given GO ---

	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			(*it)->RecursiveDelete();
			delete* it;
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

void GameObject::OnUpdateTransform()
{
	if (Static)
		return;

	ComponentTransform* transform = GetComponent<ComponentTransform>();

	if(parent)
	transform->OnUpdateTransform(parent->GetComponent<ComponentTransform>()->GetGlobalTransform());

	ComponentCamera* camera = GetComponent<ComponentCamera>();

	if(camera)
	camera->OnUpdateTransform(transform->GetGlobalTransform());

	// --- Update all children ---
	if (childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			(*it)->OnUpdateTransform();	
		}
	}

	UpdateAABB();
}

void GameObject::RemoveChildGO(GameObject * GO)
{
	// --- Remove given child from list ---
	if (GO 
		&& GO->index >= 0 
		&& GO->index < childs.size() 
		&& childs[GO->index] != nullptr 
		&& GO->GetUID() == childs[GO->index]->GetUID()) 
	{
		childs.erase(childs.begin() + GO->index);

		// --- We update the indices of the rest of childs ---
		for (int i = GO->index; i < childs.size(); ++i)
		{
			if (childs[i] != nullptr) 
				childs[i]->index -= 1;
		}

		GO->index = -1;
	}
}

void GameObject::AddChildGO(GameObject * GO, int index)
{
	// --- Add a child GO to a Game Object this ---
	if (!FindChildGO(GO)) 
	{
		// --- If it has a parent we remove it from its parent --
		if (GO->parent != nullptr)
			GO->parent->RemoveChildGO(GO);

		// --- Update parent ---
		GO->parent = this;

		// --- If index was specified, insert ---
		if (index >= 0) 
		{
			// --- Reserve needed space, note that we may leave empty spaces!!! ---
			if (index + 1 > childs.size())
				childs.resize(index + 1);

			// --- Delete element at given index ---
			if (childs[index]) 
			{
				delete childs[index];
				childs[index] = nullptr;
			}

			// --- Insert element at given index ---
			childs[index] = GO;
			GO->index = index;
		}
		// --- Else push back ---
		else 
		{
			childs.push_back(GO);
			GO->index = childs.size() - 1;
		}
	}
}

void GameObject::InsertChildGO(GameObject* GO, int index) 
{
	if (!FindChildGO(GO)) 
	{
		if (index >= 0 && index < childs.size())
		{
			// --- Remove GO from current parent's childs, if it exists --
			if (GO->parent)
				GO->parent->RemoveChildGO(GO);

			GO->parent = this;

			// --- Resize vector to fit new size ---
			childs.resize(childs.size() + 1);

			// --- Move all the GOs starting from index one position up ---
			for (int i = childs.size() - 1; i > index; --i) 
			{
				childs[i] = childs[i - 1];
				childs[i]->index = i;
			}

			// --- Insert new child ---
			childs[index] = GO;
			GO->index = index;
		}

	}
}

bool GameObject::FindChildGO(GameObject * GO)
{
	// --- Look for given GO in child list and return true if found ---
	bool ret = false;

	if (GO) 
	{
		if (GO->index >= 0 && GO->index < childs.size() && childs[GO->index]) 
			ret = GO->GetUID() == childs[GO->index]->GetUID();	
	}

	return ret;
}

Component* GameObject::AddComponent(Component::ComponentType type, int index)
{
	static_assert(static_cast<int>(Component::ComponentType::Unknown) == 4, "Component Creation Switch needs to be updated");
	Component* component = nullptr;

	// --- Check if there is already a component of the type given ---

	if (HasComponent(type) == nullptr)
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
		}

		if (component)
		{
			// --- If index was specified, insert ---
			if (index >= 0)
			{
				// --- Reserve needed space, note that we may leave empty spaces!!! ---
				if (index + 1 > components.size())
					components.resize(index + 1);


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
	else
	{
		// --- If we find a component of the same type, tell the user and return such component ---
		CONSOLE_LOG("![Warning]: The current Game Object already has a component of the type given");
		component = HasComponent(type);
	}

	return component;
}

void GameObject::RemoveComponent(Component::ComponentType type)
{
	// ---Remove component of type given from game object ---

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->GetType() == type)
		{
			std::vector<Component*>::iterator it = components.begin();
			it += i;

			components.erase(it);

			break;
		}
	}
}

Component* GameObject::HasComponent(Component::ComponentType type) const
{
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

std::vector<Component*>& GameObject::GetComponents()
{
	return components;
}

void GameObject::Enable()
{
	active = true;

	for (int i = 0; i < components.size(); ++i)
		components[i]->Enable();

	for (int i = 0; i < childs.size(); ++i)
		childs[i]->Enable();
}

void GameObject::Disable()
{
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

std::string GameObject::GetName() const
{
	return name;
}

const AABB & GameObject::GetAABB()
{
	UpdateAABB();
	return aabb;
}

const OBB & GameObject::GetOBB() const
{
	return obb;
}

bool & GameObject::GetActive()
{
	return active;
}

bool GameObject::IsEnabled() const
{
	return active;
}

void GameObject::SetName(const char* name)
{
	if (name && name != "root")
		this->name = name;
}

void GameObject::UpdateAABB()
{
	ComponentMesh* mesh = GetComponent<ComponentMesh>();
	ComponentTransform* transform = GetComponent<ComponentTransform>();

	if (mesh)
	{
		obb = mesh->GetAABB();
		obb.Transform(transform->GetGlobalTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	if(!mesh)
	{
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(1, 1, 1));
		obb = aabb;
	}
}

void GameObject::ONResourceEvent(uint uid, Resource::ResourceNotificationType type)
{
	for (uint i = 0; i < components.size(); ++i)
	{
		components[i]->ONResourceEvent(uid, type);
	}

	// MYTODO: Remove this
	if (model && type == Resource::ResourceNotificationType::Deletion && model->GetUID() == uid)
		model = nullptr;
}


