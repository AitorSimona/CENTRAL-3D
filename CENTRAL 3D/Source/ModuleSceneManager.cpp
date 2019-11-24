#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"

#include "ModuleGui.h"

#include "ModuleImporter.h"
#include "ImporterMaterial.h"
#include "ImporterScene.h"

#include "par/par_shapes.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"


#include "mmgr/mmgr.h"



ModuleSceneManager::ModuleSceneManager(bool start_enabled)
{
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init(json file)
{
	// --- Create Root GO ---
	root = CreateRootGameObject();
	tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

	return true;
}

bool ModuleSceneManager::Start()
{
	// --- Define Default and Checkers Materials ---
	DefaultMaterial = CreateEmptyMaterial();
	DefaultMaterial->resource_material = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::MATERIAL);
	DefaultMaterial->resource_material->resource_diffuse = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);

	DefaultMaterial->resource_material->resource_diffuse->Texture_path = "Default";

	CheckersMaterial = CreateEmptyMaterial();
	CheckersMaterial->resource_material = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::MATERIAL);
	CheckersMaterial->resource_material->resource_diffuse = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);

	CheckersMaterial->resource_material->resource_diffuse->buffer_id = App->textures->GetCheckerTextureID();
	CheckersMaterial->resource_material->resource_diffuse->Texture_path = "NaN";
	CheckersMaterial->resource_material->resource_diffuse->Texture_width = CHECKERS_WIDTH;
	CheckersMaterial->resource_material->resource_diffuse->Texture_height = CHECKERS_HEIGHT;

	// --- Create primitives ---
	cube = CreateCube(1, 1, 1);
	sphere = CreateSphere(1.0f, 25, 25);


	return true;
}

update_status ModuleSceneManager::PreUpdate(float dt)
{


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
	NoStaticGo.clear();

	for (uint i = 0; i < Materials.size(); ++i)
	{
		if (Materials[i])
			delete Materials[i];
	}
	Materials.clear();

	DefaultMaterial = nullptr;
	CheckersMaterial = nullptr;

	return true;
}

void ModuleSceneManager::Draw() 
{
	// --- Draw Grid ---
	CreateGrid();

	// --- Activate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Game Object Meshes ---
	DrawScene();

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleSceneManager::DrawScene()
{
	if (display_tree)
	RecursiveDrawQuadtree(tree.root);

	for (std::vector<GameObject*>::iterator it = NoStaticGo.begin(); it != NoStaticGo.end(); it++)
	{
		if ((*it)->GetName() != root->GetName())
		{
			// --- Search for Renderer Component --- 
			ComponentRenderer* Renderer = (*it)->GetComponent<ComponentRenderer>(Component::ComponentType::Renderer);

			// --- If Found, draw the mesh ---
			if (Renderer && Renderer->IsEnabled())
					Renderer->Draw();
		}
	}
	std::vector<GameObject*> static_go;
	tree.CollectIntersections(static_go, App->renderer3D->culling_camera->frustum);

	for (std::vector<GameObject*>::iterator it = static_go.begin(); it != static_go.end(); it++)
	{
		// --- Search for Renderer Component --- 
		ComponentRenderer* Renderer = (*it)->GetComponent<ComponentRenderer>(Component::ComponentType::Renderer);

		// --- If Found, draw the mesh ---
		if (Renderer && Renderer->IsEnabled())
			Renderer->Draw();
	}

	// --- Draw ray ---
	//if (App->camera->last_ray.IsFinite())
	//{
	//	glDisable(GL_LIGHTING);
	//	glBegin(GL_LINES);

	//	glColor4f(Red.r, Red.g, Red.b, Red.a);

	//	glVertex3fv((GLfloat*)&App->camera->last_ray.a);
	//	glVertex3fv((GLfloat*)&App->camera->last_ray.b);

	//	glColor4f(1.0, 1.0, 1.0, 1.0);

	//	glEnd();
	//	glEnable(GL_LIGHTING);
	//}
}

GameObject * ModuleSceneManager::GetRootGO() const
{
	return root;
}

void ModuleSceneManager::RedoOctree()
{
	std::vector<GameObject*> scene_gos;
	tree.CollectObjects(scene_gos);

	tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

	for (uint i = 0; i < scene_gos.size(); ++i)
	{
		//tree.Erase(scene_gos[i]);
		tree.Insert(scene_gos[i]);
	}

}

void ModuleSceneManager::SetStatic(GameObject * go)
{
	if (go->Static)
	{
		tree.Insert(go);

		for (std::vector<GameObject*>::iterator it = NoStaticGo.begin(); it != NoStaticGo.end(); it++)
		{
			if ((*it) == go)
			{
				NoStaticGo.erase(it);
				break;
			}
		}
	}
	else
	{
		NoStaticGo.push_back(go);
		tree.Erase(go);
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

	DrawWire(node->box, Red);
}

void ModuleSceneManager::SelectFromRay(LineSegment & ray) 
{
	// --- Note all Game Objects are pushed into a map given distance so we can decide order later ---

	if (!App->gui->IsMouseCaptured())
	{
		// --- Gather static gos ---
		std::map<float, GameObject*> candidate_gos;
		tree.CollectIntersections(candidate_gos, ray);

		// --- Gather non-static gos ---
		for (std::vector<GameObject*>::iterator it = NoStaticGo.begin(); it != NoStaticGo.end(); it++)
		{
			if (ray.Intersects((*it)->GetAABB()))
			{
				float hit_near, hit_far;
				if (ray.Intersects((*it)->GetOBB(), hit_near, hit_far))
					candidate_gos[hit_near] = *it;
			}
		}

		GameObject* toSelect = nullptr;
		for (std::map<float, GameObject*>::iterator it = candidate_gos.begin(); it != candidate_gos.end() && toSelect == nullptr; it++)
		{
			// --- We have to test triangle by triangle ---
			ComponentMesh* mesh = it->second->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

			if (mesh)
			{

				if (mesh->resource_mesh)
				{
					// --- We need to transform the ray to local mesh space ---
					LineSegment local = ray;
					local.Transform(it->second->GetComponent<ComponentTransform>(Component::ComponentType::Transform)->GetGlobalTransform().Inverted());

					for (uint j = 0; j < mesh->resource_mesh->IndicesSize / 3; j++)
					{
						float3 a = mesh->resource_mesh->Vertices[mesh->resource_mesh->Indices[j * 3]];
						float3 b = mesh->resource_mesh->Vertices[mesh->resource_mesh->Indices[(j * 3) + 1]];
						float3 c = mesh->resource_mesh->Vertices[mesh->resource_mesh->Indices[(j * 3) + 2]];
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
		if (toSelect)
			SelectedGameObject = toSelect;

	}
}

void ModuleSceneManager::SaveStatus(json & file) const
{
}

void ModuleSceneManager::LoadStatus(const json & file)
{
}

void ModuleSceneManager::SaveScene()
{
	// --- Fill vector with scene's GO's ---
	std::vector<GameObject*> scene_gos;
	GatherGameObjects(scene_gos, root);

	if (scene_gos.size() > 0)
	{
		std::string Scene_name = "SampleScene";
		App->importer->GetImporterScene()->SaveSceneToFile(scene_gos, Scene_name, SCENE);
	}
}

void ModuleSceneManager::LoadScene()
{
	std::string Scene_name = SCENES_FOLDER;
	Scene_name.append("SampleScene.scene");

	RecursiveFreeScene(root);

	if(App->fs->Exists(Scene_name.data()))
	App->importer->GetImporterScene()->Load(Scene_name.data());
}

void ModuleSceneManager::RecursiveFreeScene(GameObject* go)
{
	// --- Delete all objects except root (if go is root) ---

	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			RecursiveFreeScene(*it);
		}

		go->childs.clear();
	}

	if (go->GetName() != root->GetName())
	{
		go->Static = true;
		App->scene_manager->SetStatic(go);
		App->scene_manager->tree.Erase(go);
		delete go;
	}
}

GameObject* ModuleSceneManager::GetSelectedGameObject() const
{
	return SelectedGameObject;
}

void ModuleSceneManager::GatherGameObjects(std::vector<GameObject*>& scene_gos, GameObject* go)
{
	// --- Add all childs from go to vector ---
	if(go->GetName() != root->GetName())
	scene_gos.push_back(go);

	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			GatherGameObjects(scene_gos, *it);
		}
	}
}


void ModuleSceneManager::SetSelectedGameObject(GameObject* go)
{
	SelectedGameObject = go;
}

void ModuleSceneManager::SetTextureToSelectedGO(uint id)
{
	// --- Assign Texture to Object's Material ---
	ComponentMaterial* Material = SelectedGameObject->GetComponent<ComponentMaterial>(Component::ComponentType::Material);

	if (Material)
	{
		//Material->FreeTexture();
		Material->resource_material->resource_diffuse->buffer_id = id;
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
	GameObject* new_object = new GameObject(Name.data());
	NoStaticGo.push_back(new_object);
	// --- Add component transform ---
	new_object->AddComponent(Component::ComponentType::Transform);
	new_object->UpdateAABB();

	// --- Set Parent GO ---
	root->AddChildGO(new_object);

	if(DefaultMaterial)
		DefaultMaterial->resource_material->resource_diffuse->instances++;


	// --- Assign Default Material ---
	new_object->SetMaterial(DefaultMaterial);

	return new_object;
}

GameObject * ModuleSceneManager::CreateRootGameObject()
{
	// --- Create New Game Object Name ---
	std::string Name = "root";

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.data());

	// --- Add component transform ---
	new_object->AddComponent(Component::ComponentType::Transform);

	return new_object;
}


ComponentMaterial * ModuleSceneManager::CreateEmptyMaterial()
{
	// --- Creating Empty material to be filled out ---
	ComponentMaterial* Material = new ComponentMaterial(Component::ComponentType::Material);
	Materials.push_back(Material);

	return Material;
}


void ModuleSceneManager::LoadParMesh(par_shapes_mesh_s * mesh, ResourceMesh* new_mesh) const
{
	// --- Obtain data from par shapes mesh and load it into mesh ---

	new_mesh->IndicesSize = mesh->ntriangles * 3;
	new_mesh->VerticesSize = mesh->npoints;

	// --- Vertices ---

	new_mesh->Vertices = new float3[new_mesh->VerticesSize];

	for (uint i = 0; i < new_mesh->VerticesSize; ++i)
	{
		new_mesh->Vertices[i].x = mesh->points[3 * i];
		new_mesh->Vertices[i].y = mesh->points[(3 * i) + 1];
		new_mesh->Vertices[i].z = mesh->points[(3 * i) + 2];
	}

	new_mesh->VerticesID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float3) * new_mesh->VerticesSize, new_mesh->Vertices);

	// --- Indices ---
	new_mesh->Indices = new uint[new_mesh->IndicesSize];
	for (uint i = 0; i < new_mesh->IndicesSize; ++i)
	{
		new_mesh->Indices[i] = mesh->triangles[i];
	}
	new_mesh->IndicesID = App->renderer3D->CreateBufferFromData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * new_mesh->IndicesSize, new_mesh->Indices);

	// --- Normals ---
	if (mesh->normals)
	{
		new_mesh->NormalsSize = mesh->npoints;
		new_mesh->Normals = new float3[new_mesh->NormalsSize];

		for (uint i = 0; i < new_mesh->NormalsSize; ++i)
		{
			new_mesh->Normals[i].x = mesh->normals[3 * i];
			new_mesh->Normals[i].y = mesh->normals[(3 * i) + 1];
			new_mesh->Normals[i].z = mesh->normals[(3 * i) + 2];
		}

	}

	// --- Texture Coords ---

	new_mesh->TexCoordsSize = new_mesh->VerticesSize * 2;
	new_mesh->TexCoords = new float[new_mesh->TexCoordsSize];

	for (uint i = 0; i < new_mesh->VerticesSize; ++i)
	{
		new_mesh->TexCoords[2 * i] = mesh->tcoords[2 * i];
		new_mesh->TexCoords[(2 * i) + 1] = mesh->tcoords[(2 * i) + 1];
	}

	new_mesh->TextureCoordsID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float) * new_mesh->TexCoordsSize, new_mesh->TexCoords);


	new_mesh->CreateAABB();
	par_shapes_free_mesh(mesh);
}

void ModuleSceneManager::DrawWireFromVertices(const float3 * corners, Color color)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	glColor4f(color.r, color.g, color.b, color.a);

	//Between-planes right
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	//Between-planes left
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	//Far plane horizontal
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	//Near plane horizontal
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Near plane vertical
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Far plane vertical
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glEnd();
	glEnable(GL_LIGHTING);
}

ResourceMesh* ModuleSceneManager::CreateCube(float sizeX, float sizeY, float sizeZ)
{
	// --- Generating 6 planes and merging them to create a cube, since par shapes cube 
	// does not have uvs / normals 

	ResourceMesh* new_mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH);

	par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);
	par_shapes_mesh* top = par_shapes_create_plane(1, 1);
	par_shapes_mesh* bottom = par_shapes_create_plane(1, 1);
	par_shapes_mesh* back = par_shapes_create_plane(1, 1);
	par_shapes_mesh* left = par_shapes_create_plane(1, 1);
	par_shapes_mesh* right = par_shapes_create_plane(1, 1);

	par_shapes_translate(mesh, -0.5f, -0.5f, 0.5f);

	par_shapes_rotate(top, -float(PAR_PI*0.5), (float*)&float3::unitX);
	par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

	par_shapes_rotate(bottom, float(PAR_PI*0.5), (float*)&float3::unitX);
	par_shapes_translate(bottom, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(back, float(PAR_PI), (float*)&float3::unitX);
	par_shapes_translate(back, -0.5f, 0.5f, -0.5f);

	par_shapes_rotate(left, float(-PAR_PI * 0.5), (float*)&float3::unitY);
	par_shapes_translate(left, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(right, float(PAR_PI*0.5), (float*)&float3::unitY);
	par_shapes_translate(right, 0.5f, -0.5f, 0.5f);

	par_shapes_merge_and_free(mesh, top);
	par_shapes_merge_and_free(mesh, bottom);
	par_shapes_merge_and_free(mesh, back);
	par_shapes_merge_and_free(mesh, left);
	par_shapes_merge_and_free(mesh, right);

	if (mesh)
	{
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, new_mesh);
	}

	return new_mesh;
}

ResourceMesh* ModuleSceneManager::CreateSphere(float Radius, int slices, int slacks)
{
	ResourceMesh* new_mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH);

	// --- Create par shapes sphere ---
	par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (mesh)
	{
		par_shapes_scale(mesh, Radius / 2, Radius / 2, Radius / 2);
		LoadParMesh(mesh, new_mesh);
	}

	return new_mesh;
}

void ModuleSceneManager::CreateGrid() const
{
	// --- Create Grid in direct mode (to be changed) ---
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	float distance = 10.0f;

	for (int max_linesgrid = -distance; max_linesgrid < distance; max_linesgrid++)
	{
		glVertex3f((float)max_linesgrid, 0.0f, -distance);
		glVertex3f((float)max_linesgrid, 0.0f, distance);
		glVertex3f(-distance, 0.0f, (float)max_linesgrid);
		glVertex3f(distance, 0.0f, (float)max_linesgrid);
	}

	glVertex3f((float)-distance, 0.0f, distance);
	glVertex3f((float)distance, 0.0f, distance);
	glVertex3f((float)distance, 0.0f, -distance);
	glVertex3f((float)distance, 0.0f, distance);

	glLineWidth(1.0f);

	glEnd();
}

GameObject * ModuleSceneManager::LoadCube()
{
	GameObject* new_object = CreateEmptyGameObject();
	ComponentMesh * comp_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);
	comp_mesh->resource_mesh = cube;
	cube->instances++;
	ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);

	return new_object;
}

GameObject * ModuleSceneManager::LoadSphere()
{
	GameObject* new_object = CreateEmptyGameObject();
	ComponentMesh * comp_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);
	comp_mesh->resource_mesh = sphere;
	sphere->instances++;
	ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);

	return new_object;
}

void ModuleSceneManager::DestroyGameObject(GameObject * go)
{
	go->parent->RemoveChildGO(go);
	go->RecursiveDelete();

	this->go_count--;
}
