#include "ModuleSceneManager.h"

// -- Modules --
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEventManager.h"
#include "ModulePhysics.h"
#include "ModuleUI.h"
#include "ModuleDetour.h"
#include "ModuleSelection.h"
#include "ModuleScripting.h"
#include "ModuleGui.h"

#include "par/par_shapes.h"

// -- Components --
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMeshRenderer.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentBone.h"

// -- Resources --
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourcePrefab.h"
#include "ResourceScene.h"
#include "ResourceMesh.h"

// -- Importers --
#include "ImporterMaterial.h"
#include "ImporterScene.h"
#include "ImporterMeta.h"

#include "Component.h"
#include "ComponentButton.h"


#include "mmgr/mmgr.h"

#include "ComponentAudioSource.h"
#include "ModuleAudio.h"
#include "../Game/Assets/Sounds/Wwise_IDs.h"


#define TREE_UPDATE_PERIOD 1000

using namespace Broken;
// --- Event Manager Callbacks ---

// SELECTED TODO
void ModuleSceneManager::ONResourceSelected(const Event& e) 
{
	App->selection->ClearSelection();
	/*if (App->scene_manager->SelectedGameObject)
		App->scene_manager->SetSelectedGameObject(nullptr);*/
}

// SELECTED TODO
void ModuleSceneManager::ONGameObjectDestroyed(const Event& e) 
{
	// If destroyed GameObject is selected, erase from selected
	// MANAGED BY MODULE SELECTION
	/*for (std::vector<GameObject*>::iterator it = App->selection->selected_gameobjects.begin(); it != App->scene_manager->selected_gameobjects.end();)
	{
		if (e.go->GetUID() == (*it)->GetUID()) {
			App->scene_manager->selected_gameobjects.erase(it);
			break;
		}
		it++;
	}*/
	

	for (GameObject* obj : App->scene_manager->GetRootGO()->childs) //all objects in scene
	{
		if (obj->HasComponent(Component::ComponentType::Button)) //if has button component
		{
			ComponentButton* button = (ComponentButton*)obj->HasComponent(Component::ComponentType::Button); //single component (change when able to have multiple components of same type)
			if (button->script_obj != nullptr && button->script_obj->GetUID() == e.go->GetUID())
			{
				button->SetNullptr();
			}
		}
	}
}

// -------------------------------

ModuleSceneManager::ModuleSceneManager(bool start_enabled)
{
	name = "Scene Manager";
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init(json& file)
{
	// --- Create Root GO ---
	root = CreateRootGameObject();
	//root_selected = CreateRootSelectedGameObject();
	tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

	// --- Add Event Listeners ---
	App->event_manager->AddListener(Event::EventType::GameObject_destroyed, ONGameObjectDestroyed);
	App->event_manager->AddListener(Event::EventType::Resource_selected, ONResourceSelected);

	//Add pushbacks for componnets that can be repeated inside a gameobject
	repeatable_components.push_back((int)Component::ComponentType::Script);
	repeatable_components.push_back((int)Component::ComponentType::AudioSource);

	return true;
}

bool ModuleSceneManager::Start()
{
	// --- Always load default scene ---
	defaultScene->LoadToMemory();

	// --- Create temporal scene for play/stop ---
	temporalScene = (Resource*)new ResourceScene(App->GetRandom().Int(), "Temp/TemporalScene.scene");

	//if (App->isGame)
	//	LoadStatus(App->GetConfigFile());

	treeUpdateTimer = SDL_GetTicks();

	//music = LoadCube();
	//music->AddComponent(Component::ComponentType::AudioSource);
	//ComponentAudioSource* musicSource = (ComponentAudioSource*)music->GetComponent<ComponentAudioSource>();//GetComponent(Component::ComponentType::AudioSource);
	//musicSource->SetID(AK::EVENTS::MUSIC);
	//musicSource->wwiseGO->PlayEvent(AK::EVENTS::BACKGROUNDMUSIC);
	//musicSource->isPlaying = true;

	return true;
}

update_status ModuleSceneManager::PreUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update(float dt)
{

	root->Update(dt);

	if (update_tree)
		if ((SDL_GetTicks() - treeUpdateTimer) > TREE_UPDATE_PERIOD) {
			treeUpdateTimer = SDL_GetTicks();
			RedoOctree();
			update_tree = false;
		}

	return UPDATE_CONTINUE;
}

bool ModuleSceneManager::CleanUp()
{
	root->RecursiveDelete();

	if (temporalScene != nullptr)
		delete temporalScene;

	delete root;
	root = nullptr;

	// MYTODO: Move this to renderer


	return true;
}

void ModuleSceneManager::DrawScene()
{

	if (display_tree)
		RecursiveDrawQuadtree(tree.root);

	// MYTODO: Support multiple go selection and draw outline accordingly
	if (currentScene)
	{
		for (std::unordered_map<uint, GameObject*>::iterator it = currentScene->NoStaticGameObjects.begin(); it != currentScene->NoStaticGameObjects.end(); it++)
		{
			if ((*it).second->GetActive() && (*it).second->GetUID() != root->GetUID())
			{
				const AABB aabb = (*it).second->GetAABB();

				// Careful! Some aabbs have NaN values inside, which triggers an assert in geolib's Intersects function

				// MYTODO: Check why some aabbs have NaN values, found one with lots of them

				if (aabb.IsFinite() && App->renderer3D->culling_camera->frustum.Intersects(aabb))
				{
					// --- Issue render order ---
					if ((*it).second->GetActive())
						(*it).second->Draw();
				}
			}
		}
		std::vector<GameObject*> static_go;
		tree.CollectIntersections(static_go, App->renderer3D->culling_camera->frustum);

		for (std::vector<GameObject*>::iterator it = static_go.begin(); it != static_go.end(); it++)
		{
			// --- Issue render order ---
			if ((*it)->GetActive())
				(*it)->Draw();
		}

		App->detour->Draw();

	}

}

GameObject* ModuleSceneManager::GetRootGO() const
{
	return root;
}

void ModuleSceneManager::RedoOctree()
{
	std::vector<GameObject*> staticGameObjects;
	tree.CollectObjects(staticGameObjects);

	tree.SetBoundaries(AABB(tree.root->box));

	for (uint i = 0; i < staticGameObjects.size(); ++i)
	{
		//tree.Erase(scene_gos[i]);
		tree.Insert(staticGameObjects[i]);
	}

}

void ModuleSceneManager::RedoOctree(AABB aabb)
{
	std::vector<GameObject*> staticGameObjects;
	tree.CollectObjects(staticGameObjects);

	tree.SetBoundaries(aabb);

	for (uint i = 0; i < staticGameObjects.size(); ++i)
	{
		//tree.Erase(scene_gos[i]);
		tree.Insert(staticGameObjects[i]);
	}
}

void ModuleSceneManager::SetStatic(GameObject * go,bool setStatic, bool setChildren)
{
	go->Static = setStatic;
	if (go->Static)
	{
		// --- Insert go into octree and remove it from currentscene's static go map ---
		tree.Insert(go);
		currentScene->StaticGameObjects[go->GetUID()] = go;

		// --- Erase go from currentscene's no static map ---
		currentScene->NoStaticGameObjects.erase(go->GetUID());

		//If recursive, set the chilidren to static
		if (setChildren)
		{
			std::vector<GameObject*> children;
			go->GetAllChilds(children);

			//start the loop from 1, because the GO in the index 0 is the parent GO
			for (int i = 1; i < children.size(); ++i)
			{
				if (!children[i]->Static) {
					children[i]->Static = setStatic;

					tree.Insert(children[i]);
					currentScene->StaticGameObjects[children[i]->GetUID()] = children[i];

					currentScene->NoStaticGameObjects.erase(children[i]->GetUID());
				}
			}
		}
	}
	else
	{
		// --- Add go to currentscene's no static map ---
		currentScene->NoStaticGameObjects[go->GetUID()] = go;

		// --- Remove go from octree and currentscene's static go map ---
		tree.Erase(go);
		currentScene->StaticGameObjects.erase(go->GetUID());

		update_tree = true;

		//If recursive, set the children to non-static
		if (setChildren)
		{
			std::vector<GameObject*> children;
			go->GetAllChilds(children);

			//start the loop from 1, because the GO in the index 0 is the parent GO
			for (int i = 1; i < children.size(); ++i)
			{
				children[i]->Static = setStatic;
				currentScene->NoStaticGameObjects[children[i]->GetUID()] = children[i];

				tree.Erase(children[i]);
				currentScene->StaticGameObjects.erase(children[i]->GetUID());
			}
		}
	}
}

void ModuleSceneManager::RecursiveDrawQuadtree(QuadtreeNode* node) const
{
	if (!node->IsLeaf()) {
		for (uint i = 0; i < 8; ++i) {
			RecursiveDrawQuadtree(node->childs[i]);
		}
	}

	if (node->IsLeaf())
		App->renderer3D->DrawAABB(node->box, Red);
}

//bool ModuleSceneManager::IsSelected(GameObject* go)
//{
//	for (int i = 0; i < selected_gameobjects.size(); i++)
//	{
//		if (selected_gameobjects[i] == go)
//			return true;
//	}
//
//	return false;
//}

void ModuleSceneManager::SelectFromRay(LineSegment& ray) 
{
	// --- Note all Game Objects are pushed into a map given distance so we can decide order later ---
	if (currentScene)
	{
		// --- Gather static gos ---
		std::map<float, GameObject*> candidate_gos;
		tree.CollectIntersections(candidate_gos, ray);

		// --- Gather non-static gos ---
		for (std::unordered_map<uint, GameObject*>::iterator it = currentScene->NoStaticGameObjects.begin(); it != currentScene->NoStaticGameObjects.end(); it++)
		{
			if ((*it).second->GetActive() && ray.Intersects((*it).second->GetAABB()))
			{
				float hit_near, hit_far;
				if (ray.Intersects((*it).second->GetOBB(), hit_near, hit_far))
					candidate_gos[hit_near] = (*it).second;
			}
		}

		GameObject* toSelect = nullptr;
		for (std::map<float, GameObject*>::iterator it = candidate_gos.begin(); it != candidate_gos.end() && toSelect == nullptr; it++)
		{
			// --- We have to test triangle by triangle ---
			ComponentMesh* mesh = it->second->GetComponent<ComponentMesh>();

			if (mesh)
			{
				if (mesh->resource_mesh)
				{
					// --- We need to transform the ray to local mesh space ---
					LineSegment local = ray;
					local.Transform(it->second->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted());

					for (uint j = 0; j < mesh->resource_mesh->IndicesSize / 3; j++)
					{
						float3 a = float3(mesh->resource_mesh->vertices[mesh->resource_mesh->Indices[j * 3]].position);
						float3 b = float3(mesh->resource_mesh->vertices[mesh->resource_mesh->Indices[(j * 3) + 1]].position);
						float3 c = float3(mesh->resource_mesh->vertices[mesh->resource_mesh->Indices[(j * 3) + 2]].position);
						// --- Create Triangle given three vertices ---
						Triangle triangle(a, b, c);

						// --- Test ray/triangle intersection ---
						if (local.Intersects(triangle, nullptr, nullptr))
						{
							toSelect = it->second;
							break;
						}
					}
				}
			}
		}

		// --- Set Selected ---
		//if (toSelect)
		// RAYCAST SELECTION
		App->selection->HandleSelection(toSelect);
		//SetSelectedGameObject(toSelect);
	}
}


void ModuleSceneManager::LoadGame(const json & file)
{
	if (file["SceneManager"].find("MainScene") != file["SceneManager"].end())
	{
		std::string sceneName = file["SceneManager"]["MainScene"];

		ResourceScene* scene = nullptr;
		for (std::map<uint, Broken::ResourceScene*>::const_iterator it = App->resources->scenes.begin(); it != App->resources->scenes.end() && scene == nullptr; ++it) {
			if ((*it).second->GetName() == sceneName)
				scene = (*it).second;

		}

		if (scene != nullptr)
		{
			SetActiveScene(scene);

			if (file["Camera3D"].find("MainCamera") != file["Camera3D"].end())
			{
				GameObject* camera;
				std::string cameraName = file["Camera3D"]["MainCamera"];
				camera = scene->GetGOWithName(cameraName.c_str());

				if (camera != nullptr)
				{
					ComponentCamera* camera_component = camera->GetComponent<ComponentCamera>();
					if (camera_component != nullptr)
					{
						App->renderer3D->SetActiveCamera(camera_component);
						App->renderer3D->SetCullingCamera(camera_component);
						/*	App->renderer3D->active_camera = camera->GetComponent<ComponentCamera>();
							App->renderer3D->culling_camera = camera->GetComponent<ComponentCamera>();*/
					}
				}
			}
			else
				ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Could not find main camera for game.", );
		}
	}
	else
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Could not find main scene for game.", );
}

void ModuleSceneManager::SaveScene(ResourceScene* scene)
{
	if (scene)
	{
		ImporterScene* IScene = App->resources->GetImporter<ImporterScene>();

		// --- Create meta ---
		if (!App->resources->IsFileImported(scene->GetOriginalFile()))
		{
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, scene->GetResourceFile(), scene->GetUID());

			if (meta)
				IMeta->Save(meta);
		}

		IScene->SaveSceneToFile(scene);

		App->resources->AddResourceToFolder(scene);

	}
}

void ModuleSceneManager::SetActiveScene(ResourceScene* scene)
{
	if (scene)
	{
		App->selection->ClearSelection();
		//SelectedGameObject = nullptr;

		// --- Unload current scene ---
		if (currentScene)
		{
			App->physics->DeleteActors();

			// --- Reset octree ---
			tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

			// --- Release current scene ---
			currentScene->Release();
			App->scripting->CleanUpInstances();

			// --- Clear root ---
			root->childs.clear();

			App->ui_system->Clear();
		}

		if (App->GetAppState() == AppState::TO_EDITOR)
		{
			std::string res_path = currentScene->GetResourceFile();
			currentScene->SetResourceFile(temporalScene->GetResourceFile());
			currentScene = (ResourceScene*)App->resources->GetResource(scene->GetUID());
			currentScene->SetResourceFile(res_path.c_str());
		}
		else
		{
			currentScene = scene; // force this so gos are not added to another scene
			currentScene = (ResourceScene*)App->resources->GetResource(scene->GetUID());

			// --- Make sure to save newly loaded scene to temporal scene so we do not load a previous one on stop ---
			if (App->GetAppState() == AppState::PLAY)
			{
				App->scene_manager->temporalScene->NoStaticGameObjects.clear();
				App->scene_manager->temporalScene->StaticGameObjects.clear();
				App->scene_manager->currentScene->CopyInto(App->scene_manager->temporalScene);
				App->scene_manager->SaveScene(App->scene_manager->temporalScene);
			}
		}
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Trying to load invalid scene");

}

// SELECTED TODO
//GameObject* ModuleSceneManager::GetSelectedGameObject() const
//{
//
//	return selected_gameobjects.empty() ? nullptr : *selected_gameobjects.rbegin();
//}



// SELECTED TODO
//void ModuleSceneManager::SetSelectedGameObject(GameObject* go) {
//	
//	if (go == nullptr)
//	{
//		App->scene_manager->selected_gameobjects.clear();
//	}
//	else {
//
//		App->scene_manager->selected_gameobjects.push_back(go);
//		Event e(Event::EventType::GameObject_selected);
//		e.go = go;
//		App->event_manager->PushEvent(e);
//	}
//	
//	/*SelectedGameObject = go;
//
//	if (SelectedGameObject)
//	{
//		Event e(Event::EventType::GameObject_selected);
//		e.go = go;
//		App->event_manager->PushEvent(e);
//	}*/
//}

GameObject* ModuleSceneManager::CreateEmptyGameObject() 
{
	// --- Create New Game Object Name ---
	std::string Name = "GameObject ";
	Name.append("(");
	Name.append(std::to_string(go_count));
	Name.append(")");

	go_count++;

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.c_str());
	currentScene->NoStaticGameObjects[new_object->GetUID()] = new_object;

	if (App->gui->editingPrefab)
	{
		App->gui->prefab->parentgo->AddChildGO(new_object);
	}
	else
		App->scene_manager->GetRootGO()->AddChildGO(new_object);

	return new_object;
}

GameObject* ModuleSceneManager::CreateEmptyGameObjectGivenUID(uint UID)
{
	// --- Create New Game Object Name ---
	std::string Name = "GameObject ";
	Name.append("(");
	Name.append(std::to_string(go_count));
	Name.append(")");

	go_count++;

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.data(),UID);
	currentScene->NoStaticGameObjects[new_object->GetUID()] = new_object;

	if (App->gui->editingPrefab)
	{
		App->gui->prefab->parentgo->AddChildGO(new_object);
	}
	else
		App->scene_manager->GetRootGO()->AddChildGO(new_object);

	return new_object;
}

void ModuleSceneManager::ResetGameObjectUID(GameObject* go)
{

}

GameObject * ModuleSceneManager::CreateRootGameObject()
{
	// --- Create New Game Object Name ---
	std::string Name = "root1";

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.c_str());

	return new_object;
}

//GameObject* ModuleSceneManager::CreateRootSelectedGameObject()
//{
//	// --- Create New Game Object Name ---
//	std::string Name = "rootSelected";
//
//	// --- Create empty Game object to be filled out ---
//	GameObject* new_object = new GameObject(Name.c_str());
//
//	return new_object;
//}

void ModuleSceneManager::CalculateTangentAndBitangent(ResourceMesh* mesh, uint index, float3& tangent, float3& bitangent) const
{
	int i = index;
	float3 v0 = { mesh->vertices[i].position[0], mesh->vertices[i].position[1], mesh->vertices[i].position[2] };
	float3 v1 = { mesh->vertices[i + 1].position[0], mesh->vertices[i + 1].position[1], mesh->vertices[i + 1].position[2] };
	float3 v2 = { mesh->vertices[i + 2].position[0], mesh->vertices[i + 2].position[1], mesh->vertices[i + 2].position[2] };

	float2 uv0 = { mesh->vertices[i].texCoord[0], mesh->vertices[i].texCoord[1] };
	float2 uv1 = { mesh->vertices[i + 1].texCoord[0], mesh->vertices[i + 1].texCoord[1] };
	float2 uv2 = { mesh->vertices[i + 2].texCoord[0], mesh->vertices[i + 2].texCoord[1] };

	float3 dPos1 = v1 - v0;
	float3 dPos2 = v2 - v0;
	float2 dUV1 = uv1 - uv0;
	float2 dUV2 = uv2 - uv0;

	float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

	if (r == inf)
		r = 0.0f;

	tangent = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
	bitangent = (dPos2 * dUV1.x - dPos1 * dUV2.x) * r;
}

void ModuleSceneManager::LoadParMesh(par_shapes_mesh_s* mesh, ResourceMesh* new_mesh, bool calculateTangents) const
{
	// --- Obtain data from par shapes mesh and load it into mesh ---
	new_mesh->IndicesSize = mesh->ntriangles * 3;
	new_mesh->VerticesSize = mesh->npoints;
	new_mesh->vertices = new Vertex[new_mesh->VerticesSize];

	for (uint i = 0; i < new_mesh->VerticesSize; ++i) {
		// --- Vertices ---
		new_mesh->vertices[i].position[0] = mesh->points[3 * i];
		new_mesh->vertices[i].position[1] = mesh->points[(3 * i) + 1];
		new_mesh->vertices[i].position[2] = mesh->points[(3 * i) + 2];

		// --- Normals ---
		new_mesh->vertices[i].normal[0] = mesh->normals[3 * i];
		new_mesh->vertices[i].normal[1] = mesh->normals[(3 * i) + 1];
		new_mesh->vertices[i].normal[2] = mesh->normals[(3 * i) + 2];

		// --- Texture Coords ---
		new_mesh->vertices[i].texCoord[0] = mesh->tcoords[2 * i];
		new_mesh->vertices[i].texCoord[1] = mesh->tcoords[(2 * i) + 1];
	}

	if (calculateTangents)
	{
		for (uint i = 0; i < new_mesh->VerticesSize - 2; i += 3)
		{
			// --- Tangents & Bitangents Calculations ---	
			float3 tangent, bitangent;
			CalculateTangentAndBitangent(new_mesh, i, tangent, bitangent);

			new_mesh->vertices[i].tangent[0] = new_mesh->vertices[i + 1].tangent[0] = new_mesh->vertices[i + 2].tangent[0] = tangent.x;
			new_mesh->vertices[i].tangent[1] = new_mesh->vertices[i + 1].tangent[1] = new_mesh->vertices[i + 2].tangent[1] = tangent.y;
			new_mesh->vertices[i].tangent[2] = new_mesh->vertices[i + 1].tangent[2] = new_mesh->vertices[i + 2].tangent[2] = tangent.z;
			new_mesh->vertices[i].biTangent[0] = new_mesh->vertices[i + 1].biTangent[0] = new_mesh->vertices[i + 2].biTangent[0] = bitangent.x;
			new_mesh->vertices[i].biTangent[1] = new_mesh->vertices[i + 1].biTangent[1] = new_mesh->vertices[i + 2].biTangent[1] = bitangent.y;
			new_mesh->vertices[i].biTangent[2] = new_mesh->vertices[i + 1].biTangent[2] = new_mesh->vertices[i + 2].biTangent[2] = bitangent.z;
		}
	}

	// --- Indices ---
	new_mesh->Indices = new uint[new_mesh->IndicesSize];
	for (uint i = 0; i < new_mesh->IndicesSize; ++i)
		new_mesh->Indices[i] = mesh->triangles[i];

	par_shapes_free_mesh(mesh);
}

void ModuleSceneManager::CreateCube(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh) {
	// --- Generating 6 planes and merging them to create a cube, since par shapes cube
	// does not have uvs / normals

	par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);
	par_shapes_mesh* top = par_shapes_create_plane(1, 1);
	par_shapes_mesh* bottom = par_shapes_create_plane(1, 1);
	par_shapes_mesh* back = par_shapes_create_plane(1, 1);
	par_shapes_mesh* left = par_shapes_create_plane(1, 1);
	par_shapes_mesh* right = par_shapes_create_plane(1, 1);

	par_shapes_translate(mesh, -0.5f, -0.5f, 0.5f);

	par_shapes_rotate(top, -float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

	par_shapes_rotate(bottom, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(bottom, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(back, float(PAR_PI), (float*)&float3::unitX);
	par_shapes_translate(back, -0.5f, 0.5f, -0.5f);

	par_shapes_rotate(left, float(-PAR_PI * 0.5), (float*)&float3::unitY);
	par_shapes_translate(left, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(right, float(PAR_PI * 0.5), (float*)&float3::unitY);
	par_shapes_translate(right, 0.5f, -0.5f, 0.5f);

	par_shapes_merge_and_free(mesh, top);
	par_shapes_merge_and_free(mesh, bottom);
	par_shapes_merge_and_free(mesh, back);
	par_shapes_merge_and_free(mesh, left);
	par_shapes_merge_and_free(mesh, right);

	if (mesh) {
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, rmesh, true);
	}
}

void ModuleSceneManager::CreateSphere(float Radius, int slices, int slacks, ResourceMesh* rmesh) {
	// --- Create par shapes sphere ---
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (mesh)
	{
		par_shapes_scale(mesh, Radius / 2, Radius / 2, Radius / 2);
		LoadParMesh(mesh, rmesh, false);
	}
}

void ModuleSceneManager::CreateCylinder(float radius, float height, ResourceMesh* rmesh)
{
	// --- Create par shapes cylinder ---
	//First, create a normal cylinder and put it at (0,0,0)
	par_shapes_mesh* Cyl_PrShM = par_shapes_create_cylinder(25, 25);
	par_shapes_translate(Cyl_PrShM, 0, 0, 0);
	par_shapes_scale(Cyl_PrShM, radius/2, height/2, radius/2);

	//Now create 2 disks around the cylinder (since x, y and z are the same, we can just pick x)
	float normal[3] = { 0, 0, 1 };
	float center_axis[3] = { 0, 0, radius };
	float center_axis2[3] = { 0, 0, 1 };
	par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(radius/2, 25, center_axis, normal);
	par_shapes_mesh* Disk2_PrShM = par_shapes_create_disk(radius/2, 25, center_axis2, normal);

	//Rotate one of the disks (to make it see outside the cylinder)
	float RotAxis[3] = { 1, 0, 0 };
	par_shapes_rotate(Disk2_PrShM, PI, RotAxis);
	par_shapes_translate(Disk2_PrShM, 0, 0, 1);
	par_shapes_translate(Disk_PrShM, 0, 0, -0.5f);

	//Finally, set the class' mesh to an Empty ParShape, merge to it the 3 meshes
	par_shapes_mesh* ParshapeMesh = par_shapes_create_empty();
	par_shapes_merge_and_free(ParshapeMesh, Cyl_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk2_PrShM);

	if (ParshapeMesh)
	{
		par_shapes_scale(ParshapeMesh, radius/2, height/2, radius/2);
		LoadParMesh(ParshapeMesh, rmesh, false);
	}
}

void ModuleSceneManager::CreatePlane(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh)
{
	// --- Create par shapes sphere ---
	par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);

	if (mesh)
	{
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, rmesh, true);
	}
}

void ModuleSceneManager::CreateDisk(float radius, ResourceMesh* rmesh)
{
	// --- Create par shapes cylinder ---
		//First, create a normal cylinder and put it at (0,0,0)
	par_shapes_mesh* Cyl_PrShM = par_shapes_create_cylinder(25, 25);
	par_shapes_translate(Cyl_PrShM, 0, 0, 0);
	par_shapes_scale(Cyl_PrShM, radius / 2, 0.5, radius / 2);

	//Now create 2 disks around the cylinder (since x, y and z are the same, we can just pick x)
	float normal[3] = { 0, 0, 1 };
	float center_axis[3] = { 0, 0, radius };
	float center_axis2[3] = { 0, 0, 1 };
	par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis, normal);
	par_shapes_mesh* Disk2_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis2, normal);

	//Rotate one of the disks (to make it see outside the cylinder)
	float RotAxis[3] = { 1, 0, 0 };
	par_shapes_rotate(Disk2_PrShM, PI, RotAxis);
	par_shapes_translate(Disk2_PrShM, 0, 0, 1);
	par_shapes_translate(Disk_PrShM, 0, 0, -0.5f);

	//Finally, set the class' mesh to an Empty ParShape, merge to it the 3 meshes
	par_shapes_mesh* ParshapeMesh = par_shapes_create_empty();
	par_shapes_merge_and_free(ParshapeMesh, Cyl_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk2_PrShM);

	if (ParshapeMesh)
	{
		par_shapes_scale(ParshapeMesh, radius / 2, 0.5, 0);
		LoadParMesh(ParshapeMesh, rmesh, false);
	}
}

void ModuleSceneManager::CreateCapsule(float radius, float height, ResourceMesh* rmesh)
{

	// --- Create spheres and cylinder to build capsule ---
	par_shapes_mesh* top_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* bot_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* cylinder = par_shapes_create_cylinder(25,25);

	par_shapes_scale(top_sphere, radius / 2, radius/2 * 1/height, radius / 2);
	par_shapes_scale(bot_sphere, radius / 2, radius/2 * 1/height, radius / 2);
	par_shapes_scale(cylinder, radius / 2, radius / 2, height / 2);

	// --- Rotate and translate hemispheres ---
	par_shapes_rotate(top_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(top_sphere, 0, 0, height/2);
	par_shapes_rotate(bot_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_rotate(bot_sphere, float(PAR_PI), (float*)&float3::unitX);

	// --- Merge meshes ---
	par_shapes_merge_and_free(top_sphere, cylinder);
	par_shapes_merge_and_free(top_sphere, bot_sphere);

	// --- Position final mesh ---
	par_shapes_rotate(top_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(top_sphere, 0, height/4, 0);


	if (top_sphere)
	{
		LoadParMesh(top_sphere, rmesh, false);
	}
}

GameObject * ModuleSceneManager::LoadCube()
{
	return LoadPrimitiveObject(cube->GetUID());
}

GameObject* ModuleSceneManager::LoadPlane()
{
	return LoadPrimitiveObject(plane->GetUID());
}

GameObject* ModuleSceneManager::LoadSphere()
{
	return LoadPrimitiveObject(sphere->GetUID());
}

GameObject* ModuleSceneManager::LoadCylinder()
{
	return LoadPrimitiveObject(cylinder->GetUID());
}

GameObject* ModuleSceneManager::LoadDisk()
{
	return LoadPrimitiveObject(disk->GetUID());
}

GameObject* ModuleSceneManager::LoadCapsule()
{
	return LoadPrimitiveObject(capsule->GetUID());
}

GameObject* ModuleSceneManager::LoadPrimitiveObject(uint PrimitiveMeshID)
{
	GameObject* new_object = CreateEmptyGameObject();
	//currentScene->NoStaticGameObjects[new_object->GetUID()] = new_object;

	ComponentMesh* comp_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);
	comp_mesh->resource_mesh = (ResourceMesh*)App->resources->GetResource(PrimitiveMeshID);
	ComponentMeshRenderer* MeshRenderer = (ComponentMeshRenderer*)new_object->AddComponent(Component::ComponentType::MeshRenderer);
	MeshRenderer->material = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID());

	return new_object;
}

void ModuleSceneManager::DestroyGameObject(GameObject* go)
{
	App->physics->DeleteActors(go);
	go->parent->RemoveChildGO(go);
	go->RecursiveDelete();

	delete go;
	go = nullptr;
	this->go_count--;
}

void ModuleSceneManager::GatherGameObjects(GameObject* go, std::vector<GameObject*>& gos_vec)
{
	gos_vec.push_back(go);

	for (uint i = 0; i < go->childs.size(); ++i)
	{
		GatherGameObjects(go->childs[i], gos_vec);
	}
}

void ModuleSceneManager::SendToDelete(GameObject* go)
{
	App->physics->DeleteActors(go);

	Event e(Event::EventType::GameObject_destroyed);
	e.go = go;
	App->event_manager->PushEvent(e);

	go_to_delete.push_back(go);
}
