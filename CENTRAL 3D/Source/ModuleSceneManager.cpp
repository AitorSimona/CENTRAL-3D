#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"

#include "ImporterMaterial.h"

#include "mmgr/mmgr.h"

ModuleSceneManager::ModuleSceneManager(bool start_enabled)
{
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init(json file)
{


	return true;
}

bool ModuleSceneManager::Start()
{
	CheckersMaterial = CreateEmptyMaterial();
	CheckersMaterial->TextureID = App->textures->GetCheckerTextureID();


	CreateCube(1, 1, 1, true);

	return true;
}

update_status ModuleSceneManager::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleSceneManager::CleanUp()
{
	// --- Clean Up object data ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i])
			delete game_objects[i];
	}

	for (uint i = 0; i < Materials.size(); ++i)
	{
		if (Materials[i])
			delete Materials[i];
	}

	CheckersMaterial = nullptr;

	return true;
}


void ModuleSceneManager::Draw() const
{

	// --- Draw Grid ---
	CreateGrid();

	// --- Activate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Game Object Meshes ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		glPushMatrix();
		glMultMatrixf(game_objects[i]->GetLocalTransform().ptr());

		// --- Search for Renderer Component --- 
		ComponentRenderer* Renderer = (ComponentRenderer*)game_objects[i]->GetComponent(Component::ComponentType::Renderer);

		// --- If Found, draw the mesh ---
		if (Renderer)
		{
			Renderer->Draw();
		}

		glPopMatrix();
	}

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	

}

uint ModuleSceneManager::GetNumGameObjects() const
{
	return game_objects.size();
}

GameObject * ModuleSceneManager::CreateEmptyGameObject()
{
	// --- Create New Game Object Name ---
	std::string Name = "GameObject ";
	Name.append("(");
	Name.append(std::to_string(game_objects.size()));
	Name.append(")");

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.data());
	game_objects.push_back(new_object);

	return new_object;
}

ComponentMaterial * ModuleSceneManager::CreateEmptyMaterial()
{
	// --- Creating Empty material to be filled out ---
	ComponentMaterial* Material = new ComponentMaterial(Component::ComponentType::Material);

	Materials.push_back(Material);

	return Material;
}

GameObject * ModuleSceneManager::CreateCube(float sizeX, float sizeY, float sizeZ, bool checkers)
{
	float3 size = float3(sizeX, sizeY, sizeZ);
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	GameObject* new_object = App->scene_manager->CreateEmptyGameObject();

	ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

	ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);

	// --- Vertices ---

	uint verticesSize = 3 * 8;
	new_mesh->VerticesSize = verticesSize;

	new_mesh->Vertices = new float3[verticesSize]{  // 8 of vertex coords

		{	sx, sy, sz },  {-sx, sy, sz },  {-sx,-sy, sz },  { sx,-sy, sz },  // v0,v1,v2,v3 (front)
		{	sx, sy, sz },  { sx,-sy, sz },  { sx,-sy,-sz },  { sx, sy,-sz },  // v0,v3,v4,v5 (right)
		{	sx, sy, sz },  { sx, sy,-sz },  {-sx, sy,-sz },  {-sx, sy, sz },  // v0,v5,v6,v1 (top)
		{  -sx, sy, sz },  {-sx, sy,-sz },  {-sx,-sy,-sz },  {-sx,-sy, sz },  // v1,v6,v7,v2 (left)
		{  -sx,-sy,-sz },  { sx,-sy,-sz },  { sx,-sy, sz },  {-sx,-sy, sz },  // v7,v4,v3,v2 (bottom)
		{	sx,-sy,-sz },  {-sx,-sy,-sz },  {-sx, sy,-sz },  { sx, sy,-sz }   // v4,v7,v6,v5 (back)
	};

	glGenBuffers(1, (GLuint*)&new_mesh->VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, new_mesh->VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * verticesSize, new_mesh->Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Indices ---
	new_mesh->IndicesSize = 3 * (2 * 6);
	new_mesh->Indices = new uint[new_mesh->IndicesSize]{ 0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
									 4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
									 8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
									12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
									16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
									20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
	};


	glGenBuffers(1, (GLuint*)&new_mesh->IndicesID); // create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh->IndicesID); // start using created buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * new_mesh->IndicesSize, new_mesh->Indices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Texture Coords ---

	new_mesh->TexCoordsSize = verticesSize * 2;

	new_mesh->TexCoords = new float[new_mesh->TexCoordsSize]{
		1, 0, 0, 0, 0, 1, 1, 1,               // v0,v1,v2,v3 (front)
			0, 0, 0, 1, 1, 1, 1, 0,               // v0,v3,v4,v5 (right)
			1, 1, 1, 0, 0, 0, 0, 1,               // v0,v5,v6,v1 (top)
			1, 0, 0, 0, 0, 1, 1, 1,               // v1,v6,v7,v2 (left)
			0, 1, 1, 1, 1, 0, 0, 0,               // v7,v4,v3,v2 (bottom)
			0, 1, 1, 1, 1, 0, 0, 0                // v4,v7,v6,v5 (back)
	};

	if (checkers)
		new_object->SetMaterial(CheckersMaterial);

	glGenBuffers(1, (GLuint*)&new_mesh->TextureCoordsID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, new_mesh->TextureCoordsID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * new_mesh->TexCoordsSize, new_mesh->TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	return new_object;
}

GameObject * ModuleSceneManager::CreateSphere(float Radius, int slices, int slacks, bool checkers)
{
	//par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(slices, slacks);

	//if (mesh)
	//{
	//	par_shapes_scale(mesh, size / 2, size / 2, size / 2);

	//	IndicesSize = mesh->ntriangles * 3;
	//	verticesSize = mesh->npoints * 3;

	//	// --- Vertices ---

	//	Vertices = new float[verticesSize];

	//	for (uint i = 0; i < uint(mesh->npoints) * 3; ++i)
	//	{
	//		Vertices[i] = mesh->points[i];
	//	}

	//	glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
	//	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	//	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	//	// --- Indices ---

	//	Indices = new unsigned[IndicesSize];
	//	for (uint i = 0; i < uint(mesh->ntriangles) * 3; ++i)
	//	{
	//		Indices[i] = mesh->triangles[i];
	//	}

	//	glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * IndicesSize, mesh->triangles, GL_STATIC_DRAW); // send vertices to VRAM
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

	//	// --- Texture Coords ---

	//	if (checkers)
	//		TexID = App->textures->GetCheckerTextureID();

	//	TexCoordsSize = verticesSize * 2;
	//	TexCoords = new float[TexCoordsSize];

	//	for (uint i = 0; i < verticesSize; ++i)
	//	{
	//		TexCoords[2 * i] = mesh->tcoords[2 * i];
	//		TexCoords[(2 * i) + 1] = mesh->tcoords[(2 * i) + 1];
	//	}

	//	glGenBuffers(1, (GLuint*)&TextureCoordsID); // create buffer
	//	glBindBuffer(GL_ARRAY_BUFFER, TextureCoordsID); // start using created buffer
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * TexCoordsSize, TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
	//	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer


	//	par_shapes_free_mesh(mesh);

	//	// GL_UNSIGNED_SHORT
	//}

	return nullptr;
}

void ModuleSceneManager::CreateGrid() const
{
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
