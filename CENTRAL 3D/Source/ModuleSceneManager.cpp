#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMeshRenderer.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEventManager.h"
#include "ComponentCamera.h"

#include "ModuleGui.h"

#include "ImporterMaterial.h"
#include "ImporterScene.h"
#include "ImporterMeta.h"

#include "par/par_shapes.h"

#include "PanelHierarchy.h"
#include "ResourcePrefab.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourceScene.h"


#include "mmgr/mmgr.h"

// --- Event Manager Callbacks ---

void ModuleSceneManager::ONResourceSelected(const Event& e)
{
	if (App->scene_manager->SelectedGameObject)
		App->scene_manager->SetSelectedGameObject(nullptr);
}

void ModuleSceneManager::ONGameObjectDestroyed(const Event& e)
{
	if (e.go->GetUID() == App->scene_manager->GetSelectedGameObject()->GetUID())
		App->scene_manager->SetSelectedGameObject(nullptr);
}

// -------------------------------

ModuleSceneManager::ModuleSceneManager(bool start_enabled)
{
	name = "Scene Manager";
}

ModuleSceneManager::~ModuleSceneManager()
{
}


bool ModuleSceneManager::Init(json file)
{
	// --- Create Root GO ---
	root = CreateRootGameObject();
	tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

	// --- Add Event Listeners ---
	App->event_manager->AddListener(Event::EventType::GameObject_destroyed, ONGameObjectDestroyed);
	App->event_manager->AddListener(Event::EventType::Resource_selected, ONResourceSelected);


	return true;
}

bool ModuleSceneManager::Start()
{
	// --- Always load default scene ---
	App->scene_manager->defaultScene->LoadToMemory();

	// --- Create temporal scene for play/stop ---
	App->scene_manager->temporalScene = (Resource*)new ResourceScene(App->GetRandom().Int(), "Temp/TemporalScene.scene");

	return true;
}

update_status ModuleSceneManager::PreUpdate(float dt)
{
	// --- Delete flagged game objects ---
	if (!App->scene_manager->go_to_delete.empty())
	{
		for (uint i = 0; i < App->scene_manager->go_to_delete.size(); ++i)
		{
			DestroyGameObject(go_to_delete[i]);
		}

		App->scene_manager->go_count -= App->scene_manager->go_to_delete.size();
		App->scene_manager->go_to_delete.clear();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update(float dt)
{
	root->Update(dt);


	return UPDATE_CONTINUE;
}

bool ModuleSceneManager::CleanUp()
{
	root->RecursiveDelete();

	if (App->scene_manager->temporalScene != nullptr)
		delete App->scene_manager->temporalScene;

	delete root;
	root = nullptr;

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
					(*it).second->Draw();
				}
			}
		}
		std::vector<GameObject*> static_go;
		tree.CollectIntersections(static_go, App->renderer3D->culling_camera->frustum);

		for (std::vector<GameObject*>::iterator it = static_go.begin(); it != static_go.end(); it++)
		{
			// --- Issue render order ---
			if((*it)->GetActive())
				(*it)->Draw();
		}
	}

	//if(App->camera->last_ray.IsFinite())
	//	App->renderer3D->DrawLine(float4x4::identity, App->camera->last_ray.a, App->camera->last_ray.b, Red);
}

GameObject * ModuleSceneManager::GetRootGO() const
{
	return root;
}

void ModuleSceneManager::RedoOctree()
{
	std::vector<GameObject*> NoStaticGameObjects;
	tree.CollectObjects(NoStaticGameObjects);

	tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

	for (uint i = 0; i < NoStaticGameObjects.size(); ++i)
	{
		//tree.Erase(scene_gos[i]);
		tree.Insert(NoStaticGameObjects[i]);
	}
}

void ModuleSceneManager::SetStatic(GameObject * go)
{
	if (go->Static)
	{
		// --- Insert go into octree and remove it from currentscene's static go map ---
		tree.Insert(go);
		currentScene->StaticGameObjects[go->GetUID()] = go;

		// --- Erase go from currentscene's no static map ---
		currentScene->NoStaticGameObjects.erase(go->GetUID());
	}
	else
	{
		// --- Add go to currentscene's no static map ---
		currentScene->NoStaticGameObjects[go->GetUID()] = go;

		// --- Remove go from octree and currentscene's static go map ---
		tree.Erase(go);
		currentScene->StaticGameObjects.erase(go->GetUID());
	}
}

void ModuleSceneManager::RecursiveDrawQuadtree(QuadtreeNode * node) const
{
	if (!node->IsLeaf())
	{
		for (uint i = 0; i < 8; ++i)
		{
			RecursiveDrawQuadtree(node->childs[i]);
		}
	}

	if (node->IsLeaf())
		App->renderer3D->DrawAABB(node->box, Red);
}

void ModuleSceneManager::SelectFromRay(LineSegment & ray) 
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
				{
					if(candidate_gos.find(hit_near) == candidate_gos.end())
						candidate_gos[hit_near] = (*it).second;
					else
						candidate_gos[hit_near + 0.001] = (*it).second; // make sure we do not overwrite any object (later we will check triangles)
				}
			}
		}

		std::map<float, GameObject*> triangles_touched;

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
							// --- Check at what distance from ray origin is the triangle at ---
							float3 pos = it->second->GetComponent<ComponentTransform>()->GetGlobalTransform().TranslatePart();
							pos += triangle.CenterPoint();
							triangles_touched[float3(math::Abs(pos - ray.a)).Length()] = it->second;

							break;
						}
					}
				}
			}
		}

		// --- Get the closest triangle's GO ---
		if (triangles_touched.begin() != triangles_touched.end())
			toSelect = triangles_touched.begin()->second;

		// --- Set Selected ---
		SetSelectedGameObject(toSelect);
	}
}

void ModuleSceneManager::SaveStatus(json & file) const
{
}

void ModuleSceneManager::LoadStatus(const json & file)
{
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
		SelectedGameObject = nullptr;

		// --- Unload current scene ---
		if (currentScene)
		{
			// --- Reset octree ---
			tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

			// --- Release current scene ---
			currentScene->Release();

			// --- Clear root ---
			root->childs.clear();
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
		}
	}
	else
		CONSOLE_LOG("|[error]: Trying to load invalid scene");

}

GameObject* ModuleSceneManager::GetSelectedGameObject() const
{
	return SelectedGameObject;
}


void ModuleSceneManager::SetSelectedGameObject(GameObject* go)
{
	SelectedGameObject = go;

	if (SelectedGameObject)
	{
		Event e(Event::EventType::GameObject_selected);
		e.go = go;
		App->event_manager->PushEvent(e);
	}
}

GameObject * ModuleSceneManager::CreateEmptyGameObject() 
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

	if (App->gui->panelHierarchy->editingPrefab)
	{
		App->gui->panelHierarchy->prefab->parentgo->AddChildGO(new_object);
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
	GameObject* new_object = new GameObject(Name.data(), UID);
	currentScene->NoStaticGameObjects[new_object->GetUID()] = new_object;

	if (App->gui->panelHierarchy->editingPrefab)
	{
		App->gui->panelHierarchy->prefab->parentgo->AddChildGO(new_object);
	}
	else
		App->scene_manager->GetRootGO()->AddChildGO(new_object);

	return new_object;
}

GameObject * ModuleSceneManager::CreateRootGameObject()
{
	// --- Create New Game Object Name ---
	std::string Name = "root";

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.data());

	return new_object;
}

void ModuleSceneManager::LoadParMesh(par_shapes_mesh_s * mesh, ResourceMesh* new_mesh) const
{
	// --- Obtain data from par shapes mesh and load it into mesh ---

	new_mesh->IndicesSize = mesh->ntriangles * 3;
	new_mesh->VerticesSize = mesh->npoints;

	new_mesh->vertices = new Vertex[new_mesh->VerticesSize];

	for (uint i = 0; i < new_mesh->VerticesSize; ++i)
	{
		// --- Vertices ---
		new_mesh->vertices[i].position[0] = mesh->points[3 * i];
		new_mesh->vertices[i].position[1] = mesh->points[(3 * i) + 1];
		new_mesh->vertices[i].position[2] = mesh->points[(3 * i) + 2];

		// --- Normals ---
		new_mesh->vertices[i].normal[0] = mesh->normals[3 * i];
		new_mesh->vertices[i].normal[1] = mesh->normals[(3 * i) + 1];
		new_mesh->vertices[i].normal[2] = mesh->normals[(3 * i) + 2];

		// --- Colors ---

		// --- Texture Coords ---
		new_mesh->vertices[i].texCoord[0] = mesh->tcoords[2 * i];
		new_mesh->vertices[i].texCoord[1] = mesh->tcoords[(2 * i) + 1];
	}

	// --- Indices ---
	new_mesh->Indices = new uint[new_mesh->IndicesSize];
	for (uint i = 0; i < new_mesh->IndicesSize; ++i)
	{
		new_mesh->Indices[i] = mesh->triangles[i];
	}

	par_shapes_free_mesh(mesh);
}


void ModuleSceneManager::CreateCube(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh)
{
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

	if (mesh)
	{
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, rmesh);
	}
}

void ModuleSceneManager::CreateSphere(float Radius, int slices, int slacks, ResourceMesh* rmesh)
{
	// --- Create par shapes sphere ---
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (mesh)
	{
		par_shapes_scale(mesh, Radius / 2, Radius / 2, Radius / 2);
		LoadParMesh(mesh, rmesh);
	}
}

void ModuleSceneManager::CreatePlane(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh)
{
	// --- Create par shapes sphere ---
	par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);

	if (mesh)
	{
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, rmesh);
	}
}

void ModuleSceneManager::CreateCylinder(float radius, float height, ResourceMesh* rmesh)
{
	// --- Create par shapes cylinder ---
	//First, create a normal cylinder and put it at (0,0,0)
	par_shapes_mesh* Cyl_PrShM = par_shapes_create_cylinder(25, 25);
	par_shapes_translate(Cyl_PrShM, 0, 0, 0);
	par_shapes_scale(Cyl_PrShM, radius / 2, height / 2, radius / 2);

	//Now create 2 disks around the cylinder (since x, y and z are the same, we can just pick x)
	float normal[3] = { 0, 0, 1 };
	float center_axis[3] = { 0, 0, radius };
	float center_axis2[3] = { 0, 0, 1 };
	par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis, normal);
	par_shapes_mesh* Disk2_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis2, normal);

	//Rotate one of the disks (to make it see outside the cylinder)
	float RotAxis[3] = { 1, 0, 0 };
	par_shapes_rotate(Disk2_PrShM, math::pi, RotAxis);
	par_shapes_translate(Disk2_PrShM, 0, 0, 1);
	par_shapes_translate(Disk_PrShM, 0, 0, -0.5f);

	//Finally, set the class' mesh to an Empty ParShape, merge to it the 3 meshes
	par_shapes_mesh* ParshapeMesh = par_shapes_create_empty();
	par_shapes_merge_and_free(ParshapeMesh, Cyl_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk_PrShM);
	par_shapes_merge_and_free(ParshapeMesh, Disk2_PrShM);

	if (ParshapeMesh)
	{
		par_shapes_scale(ParshapeMesh, radius / 2, height / 2, radius / 2);
		LoadParMesh(ParshapeMesh, rmesh);
	}
}

void ModuleSceneManager::SendToDelete(GameObject* go)
{
	Event e(Event::EventType::GameObject_destroyed);
	e.go = go;
	App->event_manager->PushEvent(e);

	go_to_delete.push_back(go);
}

void ModuleSceneManager::CreateCapsule(float radius, float height, ResourceMesh* rmesh)
{
	// --- Create spheres and cylinder to build capsule ---
	par_shapes_mesh* top_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* bot_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* cylinder = par_shapes_create_cylinder(25, 25);

	par_shapes_scale(top_sphere, radius / 2, radius / 2 * 1 / height, radius / 2);
	par_shapes_scale(bot_sphere, radius / 2, radius / 2 * 1 / height, radius / 2);
	par_shapes_scale(cylinder, radius / 2, radius / 2, height / 2);

	// --- Rotate and translate hemispheres ---
	par_shapes_rotate(top_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(top_sphere, 0, 0, height / 2);
	par_shapes_rotate(bot_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_rotate(bot_sphere, float(PAR_PI), (float*)&float3::unitX);

	// --- Merge meshes ---
	par_shapes_merge_and_free(top_sphere, cylinder);
	par_shapes_merge_and_free(top_sphere, bot_sphere);

	// --- Position final mesh ---
	par_shapes_rotate(top_sphere, float(PAR_PI * 0.5), (float*)&float3::unitX);
	par_shapes_translate(top_sphere, 0, height / 4, 0);


	if (top_sphere)
	{
		LoadParMesh(top_sphere, rmesh);
	}
}

void ModuleSceneManager::CreateDisk(float radius, ResourceMesh* rmesh)
{
	// --- Create cylinder ---
	par_shapes_mesh* Cyl_PrShM = par_shapes_create_cylinder(25, 25);
	par_shapes_translate(Cyl_PrShM, 0, 0, 0);
	par_shapes_scale(Cyl_PrShM, radius / 2, 0.5, radius / 2);

	// --- Create 2 disks around the cylinder ---
	float normal[3] = { 0, 0, 1 };
	float center_axis[3] = { 0, 0, radius };
	float center_axis2[3] = { 0, 0, 1 };
	par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis, normal);
	par_shapes_mesh* Disk2_PrShM = par_shapes_create_disk(radius / 2, 25, center_axis2, normal);

	float RotAxis[3] = { 1, 0, 0 };
	par_shapes_rotate(Disk2_PrShM, math::pi, RotAxis);
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
		LoadParMesh(ParshapeMesh, rmesh);
	}
}

GameObject* ModuleSceneManager::LoadCube()
{
	return LoadPrimitive(cube->GetUID());
}

GameObject* ModuleSceneManager::LoadPlane()
{
	return LoadPrimitive(plane->GetUID());
}

GameObject* ModuleSceneManager::LoadSphere()
{
	return LoadPrimitive(sphere->GetUID());
}


GameObject* ModuleSceneManager::LoadCylinder()
{
	return LoadPrimitive(cylinder->GetUID());
}

GameObject* ModuleSceneManager::LoadDisk()
{
	return LoadPrimitive(disk->GetUID());
}

GameObject* ModuleSceneManager::LoadCapsule()
{
	return LoadPrimitive(capsule->GetUID());
}

GameObject* ModuleSceneManager::LoadPrimitive(uint UID)
{
	GameObject* new_object = CreateEmptyGameObject();

	ComponentMesh* comp_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);
	comp_mesh->resource_mesh = (ResourceMesh*)App->resources->GetResource(UID);

	ComponentMeshRenderer* MeshRenderer = (ComponentMeshRenderer*)new_object->AddComponent(Component::ComponentType::MeshRenderer);
	MeshRenderer->material = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID());

	return new_object;
}

void ModuleSceneManager::DestroyGameObject(GameObject* go)
{
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

