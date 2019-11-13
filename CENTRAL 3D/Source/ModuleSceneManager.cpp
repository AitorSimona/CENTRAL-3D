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

#include "ImporterMaterial.h"

#include "par/par_shapes.h"
#include "Math.h"


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

	return true;
}

bool ModuleSceneManager::Start()
{
	// --- Define Default and Checkers Materials ---
	DefaultMaterial = CreateEmptyMaterial();
	DefaultMaterial->Texture_path = "Default";

	CheckersMaterial = CreateEmptyMaterial();
	CheckersMaterial->TextureID = App->textures->GetCheckerTextureID();
	CheckersMaterial->Texture_path = "NaN";
	CheckersMaterial->Texture_width = CHECKERS_WIDTH;
	CheckersMaterial->Texture_height = CHECKERS_HEIGHT;

	// --- Create Cube and sphere and position them in the scene ---
	//GameObject* cube = CreateCube(1.0f, 1.0f, 1.0f);
	//GameObject* sphere = CreateSphere(1.0f,25,25);
	//cube->AddChildGO(sphere);

	//ComponentTransform* Ctransform = cube->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
	//Ctransform->SetPosition(-3.5f, 0.5f, 0.0f);
	//ComponentTransform* Stransform = sphere->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
	//Stransform->SetPosition(-6.0f, 0.5f, 0.5f);

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
	root->RecursiveDelete(root);

	for (uint i = 0; i < Materials.size(); ++i)
	{
		if (Materials[i])
			delete Materials[i];
	}
	Materials.clear();

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
	if (go->childs.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		{
			DrawRecursive(*it);
		}
	}

	if (go->GetName() != root->GetName())
	{
		ComponentTransform* transform = go->GetComponent<ComponentTransform>(Component::ComponentType::Transform);

		// --- Send transform to OpenGL and use it to draw ---
		glPushMatrix();
		glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());

		// --- Search for Renderer Component --- 
		ComponentRenderer* Renderer = go->GetComponent<ComponentRenderer>(Component::ComponentType::Renderer);

		// --- If Found, draw the mesh ---
		if (Renderer && Renderer->IsEnabled())
		{
			Renderer->Draw();
		}

		// --- Pop transform so OpenGL does not use it for other operations ---
		glPopMatrix();
	}
}

GameObject * ModuleSceneManager::GetRootGO() const
{
	return root;
}

GameObject* ModuleSceneManager::GetSelectedGameObject() const
{
	return SelectedGameObject;
}

GameObject * ModuleSceneManager::GetGoFromUID(uint UID) const
{
	return nullptr;
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
		Material->FreeTexture();
		Material->TextureID = id;
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
	ComponentMesh* new_mesh = (ComponentMesh*)new_object.AddComponent(Component::ComponentType::Mesh);

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

	par_shapes_free_mesh(mesh);
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
	go->parent->RemoveChildGO(go);
	go->RecursiveDelete(go);

	this->go_count--;
}
