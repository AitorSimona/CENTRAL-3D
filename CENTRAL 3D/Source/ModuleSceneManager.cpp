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
#include "ComponentCamera.h"

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
	tree.SetBoundaries(AABB(float3(-10, 0, -10), float3(10, 10, 10)));

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
	DrawRecursive(root);

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleSceneManager::DrawRecursive(GameObject * go)
{

	RecursiveDrawQuadtree(tree.root);


	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			DrawRecursive(*it);
		}
	}

	if (go->GetName() != root->GetName())
	{
		// --- Search for Renderer Component --- 
		ComponentRenderer* Renderer = go->GetComponent<ComponentRenderer>(Component::ComponentType::Renderer);

		// --- If Found, draw the mesh ---
		if (Renderer && Renderer->IsEnabled())
		{
			if(App->renderer3D->culling_camera->ContainsAABB(go->GetAABB())
				|| go->GetComponent<ComponentCamera>(Component::ComponentType::Camera))

				Renderer->Draw();
		}
	}
}

GameObject * ModuleSceneManager::GetRootGO() const
{
	return root;
}

void ModuleSceneManager::RedoOctree()
{
	std::vector<GameObject*> scene_gos;
	tree.CollectObjects(scene_gos);

	tree.SetBoundaries(AABB(float3(-10, 0, -10), float3(10, 10, 10)));

	for (uint i = 0; i < scene_gos.size(); ++i)
	{
		//tree.Erase(scene_gos[i]);
		tree.Insert(scene_gos[i]);
	}

}

void ModuleSceneManager::SetStatic(GameObject * go)
{
	if (!go->in_statictree)
	{
		go->in_statictree = true;
		tree.Insert(go);
	}
	if (!go->Static && go->in_statictree)
	{
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

	if(App->fs->Exists(Scene_name.data()))
	App->importer->GetImporterScene()->Load(Scene_name.data());
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
	// --- Add component transform ---
	new_object->AddComponent(Component::ComponentType::Transform);
	new_object->UpdateAABB();


	// --- Set Parent GO ---
	root->AddChildGO(new_object);

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


void ModuleSceneManager::LoadParMesh(par_shapes_mesh_s * mesh, GameObject& new_object) const
{
	// --- Obtain data from par shapes mesh and load it into component mesh ---
	ComponentMesh* comp_mesh = (ComponentMesh*)new_object.AddComponent(Component::ComponentType::Mesh);
	ResourceMesh* new_mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH);
	comp_mesh->resource_mesh = new_mesh;
	ComponentRenderer* Renderer = (ComponentRenderer*)new_object.AddComponent(Component::ComponentType::Renderer);

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

GameObject * ModuleSceneManager::CreateCube(float sizeX, float sizeY, float sizeZ) const
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

	GameObject* new_object = App->scene_manager->CreateEmptyGameObject();

	if (mesh)
	{
		par_shapes_scale(mesh, sizeX, sizeY, sizeZ);
		LoadParMesh(mesh, *new_object);
	}

	return new_object;
}

GameObject * ModuleSceneManager::CreateSphere(float Radius, int slices, int slacks) const
{
	// --- Create par shapes sphere ---
	par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(slices, slacks);

	GameObject* new_object = App->scene_manager->CreateEmptyGameObject();

	if (mesh)
	{
		par_shapes_scale(mesh, Radius / 2, Radius / 2, Radius / 2);
		LoadParMesh(mesh, *new_object);
	}

	return new_object;
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

void ModuleSceneManager::DestroyGameObject(GameObject * go)
{
	if (go->in_statictree)
		tree.Erase(go);

	go->parent->RemoveChildGO(go);
	go->RecursiveDelete();

	this->go_count--;
}
