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

#include "par/par_shapes.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"

#include "../Game/Assets/Sounds/Wwise_IDs.h"
#include "ComponentAudioSource.h"
#include "Component.h"
#include "ModuleAudio.h"

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
}

// -------------------------------

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

	// --- Add Event Listeners ---
	App->event_manager->AddListener(Event::EventType::GameObject_destroyed, ONGameObjectDestroyed);
	App->event_manager->AddListener(Event::EventType::Resource_selected, ONResourceSelected);


	return true;
}

bool ModuleSceneManager::Start()
{
	// --- Create primitives ---
	cube = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCube");
	sphere = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultSphere");
	capsule = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCapsule");
	plane = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultPlane");
	cylinder = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, "DefaultCylinder");

	CreateCube(1, 1, 1, cube);
	CreateSphere(1.0f, 25, 25, sphere);
	CreateCapsule(1, 1, capsule);
	CreatePlane(1, 1, 1, plane);
	CreateCylinder(1, 1, cylinder);

	cube->LoadToMemory();
	sphere->LoadToMemory();
	capsule->LoadToMemory();
	plane->LoadToMemory();
	cylinder->LoadToMemory();

	// --- Create adaptive grid ---
	glGenVertexArrays(1, &Grid_VAO);
	glGenBuffers(1, &Grid_VBO);
	CreateGrid(10.0f);

	glGenVertexArrays(1, &PointLineVAO);

	//Hardcoded Debug for audio
	//music = LoadCube();
	//music->AddComponent(Component::ComponentType::AudioSource);
	//ComponentAudioSource* musicSource = (ComponentAudioSource*)music->GetComponent<ComponentAudioSource>();
	//musicSource->SetID(AK::EVENTS::GERALT_ATTACK);
	//musicSource->wwiseGO->PlayEvent(AK::EVENTS::GERALT_ATTACK);
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
	return UPDATE_CONTINUE;
}

bool ModuleSceneManager::CleanUp()
{
	root->RecursiveDelete();

	delete root;
	root = nullptr;

	glDeleteVertexArrays(1, &PointLineVAO);
	glDeleteBuffers(1, (GLuint*)&Grid_VBO);
	glDeleteVertexArrays(1, &Grid_VAO);

	return true;
}

void ModuleSceneManager::DrawGrid(bool drawAxis, float size)
{
	// -------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------
	//									BY NOW, DONE IN DIRECT MODE

	//Set polygon draw mode and appropiated matrices for OGL
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();
	glMultMatrixf(float4x4::identity.Transposed().ptr());
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->camera->GetOpenGLProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->camera->GetOpenGLViewMatrix().Transposed().ptr());

	float colorIntensity = 0.65f;

	//Axis draw
	if (drawAxis)
	{
		glLineWidth(3.0f);
		glBegin(GL_LINES);

		glColor4f(colorIntensity, 0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, colorIntensity, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, colorIntensity, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();
	}

	//Plane draw
	glLineWidth(1.5f);
	glColor4f(colorIntensity, colorIntensity, colorIntensity, 1.0f);
	glBegin(GL_LINES);

	float d = size;
	for (float i = -d; i <= d; i += 1.0f)
	{
		//if ((int)i % 3 == 0)
		//	continue;

		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	//Set again Identity for OGL Matrices & Polygon draw to fill again
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// -------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------
	//									THIS IS HOW IT WAS PREVIOUSLY DONE
	// Is nice to keep this, since it was rendered in function of camera position, moving the grid with it.
	// However, as the grid doesn't has an "infinite" sensation, it was weird, so that should be fixed in order
	// for this to look good.

		/*App->renderer3D->defaultShader->use();

		GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

		float gridColor = 0.8f;
		int vertexColorLocation = glGetAttribLocation(App->renderer3D->defaultShader->ID, "color");
		glVertexAttrib3f(vertexColorLocation, gridColor, gridColor, gridColor);

		int TextureSupportLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
		glUniform1i(TextureSupportLocation, -1);

		glLineWidth(1.7f);
		glBindVertexArray(Grid_VAO);
		glDrawArrays(GL_LINES, 0, 84);
		glBindVertexArray(0);
		glLineWidth(1.0f);

		glUniform1i(TextureSupportLocation, 0);*/
}

void ModuleSceneManager::Draw()
{
	// --- Draw Grid ---
	DrawGrid(true, 75.0f);

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

	// MYTODO: Support multiple go selection and draw outline accordingly

	if (currentScene)
	{

		for (std::unordered_map<uint, GameObject*>::iterator it = currentScene->NoStaticGameObjects.begin(); it != currentScene->NoStaticGameObjects.end(); it++)
		{
			if ((*it).second->GetName() != root->GetName())
			{
				// --- Search for Renderer Component ---
				ComponentMeshRenderer* MeshRenderer = (*it).second->GetComponent<ComponentMeshRenderer>();

				if (SelectedGameObject == (*it).second)
				{
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);
				}

				// --- If Found, draw the mesh ---
				if (MeshRenderer && MeshRenderer->IsEnabled() && (*it).second->GetActive())
					MeshRenderer->Draw();

				if (SelectedGameObject == (*it).second)
				{
					glStencilMask(0x00);
				}
			}
		}
		std::vector<GameObject*> static_go;
		tree.CollectIntersections(static_go, App->renderer3D->culling_camera->frustum);

		for (std::vector<GameObject*>::iterator it = static_go.begin(); it != static_go.end(); it++)
		{
			// --- Search for Renderer Component ---
			ComponentMeshRenderer* MeshRenderer = (*it)->GetComponent<ComponentMeshRenderer>();


			if (SelectedGameObject == (*it))
			{
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
			}

			// --- If Found, draw the mesh ---
			if (MeshRenderer && MeshRenderer->IsEnabled() && (*it)->GetActive())
				MeshRenderer->Draw();

			if (SelectedGameObject == (*it))
			{
				glStencilMask(0x00);
			}
		}

	}

}

GameObject * ModuleSceneManager::GetRootGO() const
{
	return root;
}

uint ModuleSceneManager::GetPointLineVAO() const
{
	return PointLineVAO;
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

	DrawWire(node->box, Red, GetPointLineVAO());
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
		IScene->SaveSceneToFile(scene);
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

		currentScene = scene; // force this so gos are not added to another scene
		currentScene = (ResourceScene*)App->resources->GetResource(scene->GetUID());
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Trying to load invalid scene");

}

GameObject* ModuleSceneManager::GetSelectedGameObject() const
{
	return SelectedGameObject;
}


void ModuleSceneManager::SetSelectedGameObject(GameObject* go)
{
	SelectedGameObject = go;

	// MYTODO: Temporal adjustment for GameObject deselection
	//if (SelectedGameObject)
	//{
		Event e(Event::EventType::GameObject_selected);
		e.go = go;
		App->event_manager->PushEvent(e);
	//}
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
	currentScene->NoStaticGameObjects[new_object->GetUID()] = new_object;

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

	App->scene_manager->GetRootGO()->AddChildGO(new_object);

	return new_object;
}

void ModuleSceneManager::ResetGameObjectUID(GameObject* go)
{

}

GameObject * ModuleSceneManager::CreateRootGameObject()
{
	// --- Create New Game Object Name ---
	std::string Name = "root";

	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject(Name.c_str());

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

void ModuleSceneManager::DrawWireFromVertices(const float3 * corners, Color color, uint VAO)
{
	float3 vertices[24] =
	{
		//Between-planes right
		corners[1],
		corners[5],
		corners[7],
		corners[3],

		//Between-planes left
		corners[4],
		corners[0],
		corners[2],
		corners[6],

		// Far plane horizontal
		corners[5],
		corners[4],
		corners[6],
		corners[7],

		//Near plane horizontal
		corners[0],
		corners[1],
		corners[3],
		corners[2],

		//Near plane vertical
		corners[1],
		corners[3],
		corners[0],
		corners[2],

		//Far plane vertical
		corners[5],
		corners[7],
		corners[4],
		corners[6]
	};

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->linepointShader->ID);

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	int vertexColorLocation = glGetAttribLocation(App->renderer3D->linepointShader->ID, "color");
	glVertexAttrib3f(vertexColorLocation, color.r, color.g, color.b);

	// --- Create VAO, VBO ---
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// --- Draw lines ---

	glLineWidth(3.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
	glLineWidth(1.0f);

	// --- Delete VBO ---
	glDeleteBuffers(1, &VBO);

	glUseProgram(App->renderer3D->defaultShader->ID);
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

	par_shapes_rotate(top, -float(PAR_PI*0.5), (float*)&float3::unitX);
	par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

	par_shapes_rotate(bottom, float(PAR_PI*0.5), (float*)&float3::unitX);
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
	par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (mesh)
	{
		par_shapes_scale(mesh, Radius / 2, Radius / 2, Radius / 2);
		LoadParMesh(mesh, rmesh);
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
		LoadParMesh(ParshapeMesh, rmesh);
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

void ModuleSceneManager::CreateCapsule(float radius, float height, ResourceMesh* rmesh)
{
	// --- Create spheres and cylinder to build capsule ---
	par_shapes_mesh* top_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* bot_sphere = par_shapes_create_hemisphere(25, 25);
	par_shapes_mesh* cylinder = par_shapes_create_cylinder(25,25);
	par_shapes_scale(top_sphere, radius / 2, radius / 2, radius / 2);
	par_shapes_scale(bot_sphere, radius / 2, radius / 2, radius / 2);
	par_shapes_scale(cylinder, radius / 2, height/2, radius / 2);

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
	par_shapes_translate(top_sphere, 0, height/4, 0);

	if (top_sphere)
	{
		LoadParMesh(top_sphere, rmesh);
	}
}

void ModuleSceneManager::CreateGrid(float target_distance)
{
	// --- Fill vertex data ---

	float distance = target_distance/4;

	if (distance < 1)
		distance = 1;

	float3 vertices[84];

	uint i = 0;
	int lines = -10;

	for (i = 0; i < 20; i++)
	{
		vertices[4 * i] = float3 (lines*-distance, 0.0f, 10*-distance);
		vertices[4 * i + 1] = float3(lines*-distance, 0.0f, 10*distance);
		vertices[4 * i + 2] = float3(10*-distance, 0.0f, lines*distance);
		vertices[4 * i + 3] = float3(10*distance, 0.0f, lines*distance);

		lines++;
	}

	vertices[4 * i] = float3(lines * -distance, 0.0f, 10 * -distance);
	vertices[4 * i + 1] = float3(lines * -distance, 0.0f, 10 * distance);
	vertices[4 * i + 2] = float3(10 * -distance, 0.0f, lines * distance);
	vertices[4 * i + 3] = float3(10 * distance, 0.0f, lines * distance);

	// --- Configure vertex attributes ---

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(Grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

GameObject* ModuleSceneManager::LoadCapsule()
{
	return LoadPrimitiveObject(capsule->GetUID());
}

GameObject* ModuleSceneManager::LoadPrimitiveObject(uint PrimitiveMeshID)
{
	GameObject* new_object = CreateEmptyGameObject();
	ComponentMesh* comp_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);
	comp_mesh->resource_mesh = (ResourceMesh*)App->resources->GetResource(PrimitiveMeshID);

	ComponentMeshRenderer* MeshRenderer = (ComponentMeshRenderer*)new_object->AddComponent(Component::ComponentType::MeshRenderer);
	MeshRenderer->material = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID());

	return new_object;
}

void ModuleSceneManager::DestroyGameObject(GameObject * go)
{
	go->parent->RemoveChildGO(go);
	go->RecursiveDelete();
	delete go;
	this->go_count--;
}
