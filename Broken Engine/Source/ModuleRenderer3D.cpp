#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleParticles.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMeshRenderer.h"
#include "ComponentCollider.h"
#include "ComponentCharacterController.h"
#include "ResourceShader.h"
#include "ComponentAudioListener.h"
#include "ComponentLight.h"
#include "Component.h"
#include "ComponentParticleEmitter.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "PanelScene.h"

#include "Imgui/imgui.h"
#include "OpenGL.h"
#include "Math.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "mmgr/mmgr.h"

using namespace Broken;

// ---------------------------------------------------------------------------------------------
// ------------------------------ Module -------------------------------------------------------
// ---------------------------------------------------------------------------------------------
ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled) {
	name = "Renderer3D";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D() {
}

// Called before render is available
bool ModuleRenderer3D::Init(json& file)
{
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if (context == NULL)
	{
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
			ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		// Initialize glad
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Error initializing glad! %s\n");
			ret = false;
		}
		else
		{
			GL_SETERRORHANDLER(4, 4); //OpenGL Error Handler
			LoadStatus(file);
		}
	}

	// --- z values from 0 to 1 and not -1 to 1, more precision in far ranges ---
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	// --- Enable stencil testing, set to replace ---
	glEnable(GL_DEPTH_TEST); //For shadows
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

	// --- Create screen quad ---
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// --- Check if graphics driver supports shaders in binary format ---
	//GLint formats = 0;
	//glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	//if (formats < 1) {
	//	std::cerr << "Driver does not support any binary formats." << std::endl;
	//	exit(EXIT_FAILURE);
	//}

	ENGINE_AND_SYSTEM_CONSOLE_LOG("OpenGL Version: %s", glGetString(GL_VERSION));
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Glad Version: 0.1.33"); //Glad has no way to check its version


	//Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

	// --- Create adaptive grid ---
	glGenVertexArrays(1, &Grid_VAO);
	glGenBuffers(1, &Grid_VBO);
	CreateGrid(10.0f);

	glGenVertexArrays(1, &PointLineVAO);

	// --- Create camera to take model/meshes screenshots ---
	screenshot_camera = new ComponentCamera(nullptr);
	screenshot_camera->name = "Screenshot";
	screenshot_camera->frustum.SetPos(float3(0.0f, 25.0f, -50.0f));
	screenshot_camera->SetFOV(60.0f);
	screenshot_camera->Look({ 0.0f, 0.0f, 0.0f });

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	// --- Update OpenGL Capabilities ---
	UpdateGLCapabilities();

	// --- Clear stencil buffer, enable write ---
	glStencilMask(0xFF);
	glClearStencil(0);

	// --- Clear framebuffers ---
	float backColor = 0.65f;
	glClearColor(backColor, backColor, backColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearDepth(0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(backColor, backColor, backColor, 1.0f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	// --- Set Shader Matrices ---
	GLint viewLoc = glGetUniformLocation(defaultShader->ID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix (just different standard)
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(defaultShader->ID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	GLint modelLoc = glGetUniformLocation(defaultShader->ID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.Transposed().ptr());

	GLint camPosLoc = glGetUniformLocation(defaultShader->ID, "u_CameraPosition");
	float3 camPos = App->renderer3D->active_camera->GetCameraPosition();
	glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

	// --- Bind fbo ---
    if (renderfbo)
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// --- Set depth filter to greater (Passes if the incoming depth value is greater than the stored depth value) ---
	glDepthFunc(GL_GREATER);

	// --- Do not write to the stencil buffer ---
	glStencilMask(0x00);

	// --- Issue Render orders ---
	App->scene_manager->DrawScene();

	// --- Draw Grid ---
	if (display_grid)
		DrawGrid();

	// --- Draw ---
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SendShaderUniforms(defaultShader->ID);
	DrawRenderMeshes();
	DrawRenderLines();
	DrawRenderBoxes();

	// -- Draw particles ---
	for (int i = 0; i < particleEmitters.size(); ++i)
		particleEmitters[i]->DrawParticles();

	glDisable(GL_BLEND);

	std::vector<ComponentLight*>::iterator LightIterator = m_LightsVec.begin();
	for (; LightIterator != m_LightsVec.end(); ++LightIterator)
		(*LightIterator)->Draw();

	App->ui_system->Draw();


	// --- Selected Object Outlining ---
	//#ifndef BE_GAME_BUILD
	HandleObjectOutlining();
	/*#endif*/

	// --- Back to defaults ---
	glDepthFunc(GL_LESS);

	// --- Unbind fbo ---
	if (renderfbo)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// -- Draw framebuffer texture ---
	if (drawfb)
		DrawFramebuffer();

	// --- Draw GUI and swap buffers ---
	App->gui->Draw();


	// --- To prevent problems with viewports, disabled due to crashes and conflicts with docking, sets a window as current rendering context ---
	SDL_GL_MakeCurrent(App->window->window, context);
	SDL_GL_SwapWindow(App->window->window);

	// --- Clear render orders ---
	ClearRenderOrders();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Destroying 3D Renderer");

	m_LightsVec.clear();

	delete screenshot_camera;

	glDeleteBuffers(1, (GLuint*)&Grid_VBO);
	glDeleteVertexArrays(1, &Grid_VAO);

	glDeleteBuffers(1, (GLuint*)&quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	glDeleteFramebuffers(1, &fbo);
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::LoadStatus(const json& file)
{
	m_GammaCorrection = file["Renderer3D"]["GammaCorrection"].is_null() ? 1.0f : file["Renderer3D"]["GammaCorrection"].get<float>();
}

const json& ModuleRenderer3D::SaveStatus() const
{
	static json m_config;
	m_config["GammaCorrection"] = m_GammaCorrection;
	return m_config;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	// --- Called by UpdateWindowSize() in Window module this when resizing windows to prevent rendering issues ---

	// --- Resetting View matrices ---
	glViewport(0, 0, width, height);

	if (width > height)
		active_camera->SetAspectRatio(width / height);
	else
		active_camera->SetAspectRatio(height / width);

	glDeleteFramebuffers(1, &fbo);
	CreateFramebuffer();
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Setters ------------------------------------------------------
// ---------------------------------------------------------------------------------------------
bool ModuleRenderer3D::SetVSync(bool _vsync)
{
	bool ret = true;

	vsync = _vsync;

	if (vsync) {

		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			ret = false;
			ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
	}
	else {

		if (SDL_GL_SetSwapInterval(0) == -1)
		{
			ret = false;
			ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Warning: Unable to set immediate updates! SDL Error: %s\n", SDL_GetError());
		}
	}

	return ret;
}

void ModuleRenderer3D::SetActiveCamera(ComponentCamera* camera)
{
	if (this->active_camera)
		this->active_camera->active_camera = false;

	// if camera is not nullptr, then we set it as active camera, else we set editor camera as active camera
	if (camera != nullptr)
	{
		this->active_camera = camera;
		camera->active_camera = true;
	}
	else
		this->active_camera = App->camera->camera;
}

void ModuleRenderer3D::SetCullingCamera(ComponentCamera* camera)
{
	if (culling_camera)
	{
		culling_camera->culling = false;
	}
	// if camera is not nullptr, then we set it as culling camera, else we set editor camera as culling camera
	this->culling_camera = camera ? camera : App->camera->camera;
	if (camera)
	{
		camera->culling = true;
	}
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Render Commands ----------------------------------------------
// ---------------------------------------------------------------------------------------------

// --- Add render order to queue ---
void ModuleRenderer3D::DrawMesh(const float4x4 transform, const ResourceMesh* mesh, ResourceMaterial* mat, const ResourceMesh* deformable_mesh, const RenderMeshFlags flags, const Color& color)
{
	// --- Check data validity
	if (transform.IsFinite() && mesh && mat)
	{
		// --- Add given instance to relevant vector ---
		if (render_meshes.find(mesh->GetUID()) != render_meshes.end())
		{
			RenderMesh rmesh = RenderMesh(transform, mesh, mat, flags/*, color*/);
			rmesh.deformable_mesh = deformable_mesh; // TEMPORAL!
			rmesh.color = color;

			//// --- Search for Character Controller Component ---
			//ComponentCharacterController* cct = App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentCharacterController>();

			//// --- If Found, draw Character Controller shape ---
			//if (cct && cct->IsEnabled())
			//	cct->Draw();

			render_meshes[mesh->GetUID()].push_back(rmesh);
		}
		else
		{
			// --- Build new vector to store mesh's instances ---
			std::vector<RenderMesh> new_vec;

			RenderMesh rmesh = RenderMesh(transform, mesh, mat, flags/*, color*/);
			rmesh.deformable_mesh = deformable_mesh; // TEMPORAL!
			rmesh.color = color;

			new_vec.push_back(rmesh);
			render_meshes[mesh->GetUID()] = new_vec;
		}
	}
}

void ModuleRenderer3D::DrawLine(const float4x4 transform, const float3 a, const float3 b, const Color& color)
{
	render_lines.push_back(RenderLine(transform, a, b, color));
}

void ModuleRenderer3D::DrawAABB(const AABB& box, const Color& color)
{
	if (box.IsFinite())
		render_aabbs.push_back(RenderBox<AABB>(&box, color));
}

void ModuleRenderer3D::DrawOBB(const OBB& box, const Color& color)
{
	if (box.IsFinite())
		render_obbs.push_back(RenderBox<OBB>(&box, color));
}

void ModuleRenderer3D::DrawFrustum(const Frustum& box, const Color& color)
{
	if (box.IsFinite())
		render_frustums.push_back(RenderBox<Frustum>(&box, color));
}


const std::string & ModuleRenderer3D::RenderSceneToTexture(std::vector<GameObject*>& scene_gos, uint& texId)
{
	if (scene_gos.size() == 0)
		return std::string("");

	// --- Issue render calls and obtain an AABB that encloses all meshes ---

	AABB aabb;
	aabb.SetNegativeInfinity();

	for (uint i = 0; i < scene_gos.size(); ++i)
	{
		scene_gos[i]->Draw();
		aabb.Enclose(scene_gos[i]->GetAABB());
	}

	// --- Frame aabb ---
	screenshot_camera->frustum.SetPos(float3(0.0f, 25.0f, -50.0f));
	screenshot_camera->SetFOV(60.0f);
	screenshot_camera->Look({ 0.0f, 0.0f, 0.0f });

	float3 center = aabb.CenterPoint();

	ComponentCamera* previous_cam = active_camera;
	SetActiveCamera(screenshot_camera);

	float diagonal = aabb.Diagonal().Length() * 0.75f;

	float3 Movement = active_camera->frustum.Front() * (diagonal);

	if (Movement.IsFinite())
		screenshot_camera->frustum.SetPos(center - Movement);

	PreUpdate(0.0f);

	//// --- Set Shader Matrices ---
	//GLint viewLoc = glGetUniformLocation(defaultShader->ID, "view");
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	//float nearp = App->renderer3D->active_camera->GetNearPlane();

	//// right handed projection matrix (just different standard)
	//float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	//float4x4 proj_RH(
	//	f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
	//	0.0f, f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, -1.0f,
	//	0.0f, 0.0f, nearp, 0.0f);

	//GLint projectLoc = glGetUniformLocation(defaultShader->ID, "projection");
	//glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	//GLint modelLoc = glGetUniformLocation(defaultShader->ID, "model_matrix");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.Transposed().ptr());

	// --- Bind fbo ---
	if (renderfbo)
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// --- Set depth filter to greater (Passes if the incoming depth value is greater than the stored depth value) ---
	glDepthFunc(GL_GREATER);

	// --- Do not write to the stencil buffer ---
	glStencilMask(0x00);

	//// --- Draw Grid ---
	//DrawGrid();

	SendShaderUniforms(defaultShader->ID);

	// --- Draw ---
	DrawRenderMeshes();

	// --- Back to defaults ---
	glDepthFunc(GL_LESS);

	// --- Unbind fbo ---
	if (renderfbo)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- Clear render orders ---
	ClearRenderOrders();

	SDL_Surface* surface = SDL_GetWindowSurface(App->window->window);

	GLubyte* pixels = new GLubyte[surface->w * surface->h * 3];

	glBindTexture(GL_TEXTURE_2D, rendertexture);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	static std::string out_path;
	uint uid = App->GetRandom().Int();
	texId = App->textures->CreateTextureFromPixels(GL_RGB, surface->w, surface->h, GL_RGB, pixels);
	App->textures->CreateAndSaveTextureFromPixels(uid, GL_RGB, surface->w, surface->h, GL_RGB, (void*)pixels, out_path);

	delete[] pixels;

	SetActiveCamera(previous_cam);

	return out_path;
}

void ModuleRenderer3D::ClearRenderOrders()
{
	render_meshes.clear();
	render_obbs.clear();
	render_aabbs.clear();
	render_frustums.clear();
	render_lines.clear();
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Draw Commands ------------------------------------------------
// ---------------------------------------------------------------------------------------------
void ModuleRenderer3D::DrawRenderMeshes()
{
	// --- Activate wireframe mode ---
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Game Object Meshes ---
	for (std::map<uint, std::vector<RenderMesh>>::const_iterator it = render_meshes.begin(); it != render_meshes.end(); ++it)
	{
		DrawRenderMesh((*it).second);
	}

	// --- DeActivate wireframe mode ---
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void ModuleRenderer3D::DrawRenderMesh(std::vector<RenderMesh> meshInstances)
{
	// --- Meshes ---
	for (uint i = 0; i < meshInstances.size(); ++i)
	{
		uint shader = defaultShader->ID;
		RenderMesh* mesh = &meshInstances[i];
		float4x4 model = mesh->transform;
		float3 colorToDraw = float3(1.0f);

		// --- Select/Outline ---
		if (mesh->flags & RenderMeshFlags_::selected)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		if (mesh->flags & RenderMeshFlags_::outline)
		{
			shader = OutlineShader->ID;
			colorToDraw = { 1.0f, 0.65f, 0.0f };
			float3 scale = float3(1.05f, 1.05f, 1.05f);
			model = float4x4::FromTRS(model.TranslatePart(), model.RotatePart(), scale);
		}

		// --- Display Z buffer ---
		if (zdrawer)
			shader = ZDrawerShader->ID;

		// --- Get Mesh Material ---
		if (mesh->mat->shader && shader != OutlineShader->ID && !zdrawer)
		{
			shader = mesh->mat->shader->ID;
			mesh->mat->UpdateUniforms();
		}

		// --- Draw Wireframe if we must ---
		if (mesh->flags & RenderMeshFlags_::wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// ------------------------ Shader Stuff ------------------------
		glUseProgram(shader);

		// --- Set Normal Mapping Draw ---
		glUniform1i(glGetUniformLocation(shader, "u_DrawNormalMapping_Lit"), (int)m_Draw_normalMapping_Lit);
		glUniform1i(glGetUniformLocation(shader, "u_DrawNormalMapping_Lit_Adv"), (int)m_Draw_normalMapping_Lit_Adv);

		// --- Gamma Correction Value ---
		glUniform1f(glGetUniformLocation(shader, "u_GammaCorrection"), m_GammaCorrection);

		// --- Set Textures usage to 0 ---
		//glUniform1i(glGetUniformLocation(shader, "u_HasDiffuseTexture"), 0);
		//glUniform1i(glGetUniformLocation(shader, "u_HasSpecularTexture"), 0);
		//glUniform1i(glGetUniformLocation(shader, "u_HasNormalMap"), 0);

		// --- Send Color ---
		glUniform3f(glGetUniformLocation(shader, "u_Color"), colorToDraw.x, colorToDraw.y, colorToDraw.z);

		// --- Set Model Matrix Uniform ---
		glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, model.Transposed().ptr());

		// --- General Rendering Uniforms (material - texture - color related) ---
		if (mesh->resource_mesh->vertices && mesh->resource_mesh->Indices) // if mesh to draw
		{
			const ResourceMesh* rmesh = mesh->resource_mesh;
			if (mesh->deformable_mesh)
				rmesh = mesh->deformable_mesh;

			// Material
			if (mesh->mat)
			{
				glUniform1f(glGetUniformLocation(shader, "u_Shininess"), mesh->mat->m_Shininess);
				glUniform3f(glGetUniformLocation(shader, "u_Color"), mesh->mat->m_AmbientColor.x, mesh->mat->m_AmbientColor.y, mesh->mat->m_AmbientColor.z);

				//Textures
				glUniform1i(glGetUniformLocation(shader, "u_UseTextures"), (int)mesh->mat->m_UseTexture);

				if (mesh->flags & RenderMeshFlags_::texture)
				{
					if (mesh->flags & RenderMeshFlags_::checkers)
						glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
					else
					{
						if (!mesh->mat->m_DiffuseResTexture && !mesh->mat->m_SpecularResTexture && !mesh->mat->m_NormalResTexture)
							glUniform1i(glGetUniformLocation(shader, "u_UseTextures"), 0);

						// Diffuse/Albedo
						if (mesh->mat->m_DiffuseResTexture)
						{
							glUniform1i(glGetUniformLocation(shader, "u_HasDiffuseTexture"), 1);

							glUniform1i(glGetUniformLocation(shader, "u_AlbedoTexture"), 1);
							glActiveTexture(GL_TEXTURE0 + 1);
							glBindTexture(GL_TEXTURE_2D, mesh->mat->m_DiffuseResTexture->GetTexID());
						}
						else
						{
							glUniform1i(glGetUniformLocation(shader, "u_AlbedoTexture"), 0);
							glUniform1i(glGetUniformLocation(shader, "u_HasDiffuseTexture"), 0);
						}

						// Specular
						if (mesh->mat->m_SpecularResTexture)
						{
							glUniform1i(glGetUniformLocation(shader, "u_HasSpecularTexture"), 1);

							glUniform1i(glGetUniformLocation(shader, "u_SpecularTexture"), 2);
							glActiveTexture(GL_TEXTURE0 + 2);
							glBindTexture(GL_TEXTURE_2D, mesh->mat->m_SpecularResTexture->GetTexID());
						}
						else
						{
							glUniform1i(glGetUniformLocation(shader, "u_SpecularTexture"), 0);
							glUniform1i(glGetUniformLocation(shader, "u_HasSpecularTexture"), 0);
						}

						// Normal
						if (mesh->mat->m_NormalResTexture)
						{
							glUniform1i(glGetUniformLocation(shader, "u_HasNormalMap"), 1);

							glUniform1i(glGetUniformLocation(shader, "u_NormalTexture"), 3);
							glActiveTexture(GL_TEXTURE0 + 3);
							glBindTexture(GL_TEXTURE_2D, mesh->mat->m_NormalResTexture->GetTexID());
						}
						else
						{
							glUniform1i(glGetUniformLocation(shader, "u_HasNormalMap"), 0);
							glUniform1i(glGetUniformLocation(shader, "u_HasNormalMap"), 0);
						}
					}
				}
				else if (mesh->flags & color) {
					glUniform3f(glGetUniformLocation(shader, "u_Color"), mesh->color.r / 255, mesh->color.g / 255, mesh->color.b / 255);
					glUniform1i(glGetUniformLocation(shader, "u_UseTextures"), (int)false);
				}
				else
					glUniform1i(glGetUniformLocation(shader, "u_UseTextures"), (int)false);

			}

			// --- Render ---
			glBindVertexArray(rmesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->EBO);
			glDrawElements(GL_TRIANGLES, rmesh->IndicesSize, GL_UNSIGNED_INT, NULL); //render from array data

			// --- Unbind Buffers ---
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE0);
		}

		// --- DeActivate wireframe mode ---
		if (mesh->flags & RenderMeshFlags_::wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (mesh->flags & RenderMeshFlags_::selected)
			glStencilMask(0x00);

		// --- Set color back to default ---
		glUniform3f(glGetUniformLocation(shader, "u_Color"), 1.0f, 1.0f, 1.0f);
	}

	glUseProgram(0);
}


void ModuleRenderer3D::SendShaderUniforms(uint shader)
{
	// --- Display Z buffer ---
	if (zdrawer)
		shader = ZDrawerShader->ID;

	glUseProgram(shader);

	// --- Give ZDrawer near and far camera frustum planes pos ---
	if (zdrawer)
		glUniform2f(glGetUniformLocation(shader, "nearfar"), active_camera->GetNearPlane(), active_camera->GetFarPlane());

	// --- Set Matrix Uniforms ---
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_View"), 1, GL_FALSE, active_camera->GetOpenGLViewMatrix().ptr());
	glUniform1f(glGetUniformLocation(shader, "time"), App->time->time);

	//// Right handed projection matrix
	float f = 1.0f / tan(active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, active_camera->GetNearPlane(), 0.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Proj"), 1, GL_FALSE, proj_RH.ptr());

	// --- Set Normal Mapping Draw ---
	glUniform1i(glGetUniformLocation(shader, "u_DrawNormalMapping"), (int)m_Draw_normalMapping);

	// --- Set HasTextures to none ---
	glUniform1i(glGetUniformLocation(shader, "u_HasDiffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(shader, "u_HasSpecularTexture"), 0);
	glUniform1i(glGetUniformLocation(shader, "u_HasNormalMap"), 0);



	if (shader == defaultShader->ID)
	{
		// --- Send Lights ---
		glUniform1i(glGetUniformLocation(shader, "u_LightsNumber"), m_LightsVec.size());
		for (uint i = 0; i < m_LightsVec.size(); ++i)
			m_LightsVec[i]->SendUniforms(shader, i);
	}
	else
		glUniform1i(glGetUniformLocation(shader, "u_LightsNumber"), 0);
}


void ModuleRenderer3D::DrawFramebuffer()
{
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(screenShader->ID);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, rendertexture);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// --- Unbind buffers ---
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

// ---------------------------------------------------------------------------------------------
// ------------------------------ Lights -------------------------------------------------------
// ---------------------------------------------------------------------------------------------
void ModuleRenderer3D::AddLight(ComponentLight* light)
{
	if (light)
		m_LightsVec.push_back(light);
}

void ModuleRenderer3D::PopLight(ComponentLight* light)
{
	if (light)
	{
		std::vector<ComponentLight*>::iterator it = std::find(m_LightsVec.begin(), m_LightsVec.end(), light);

		if (it != m_LightsVec.end())
			m_LightsVec.erase(it);
	}
}

const int ModuleRenderer3D::GetLightIndex(ComponentLight* light)
{
	if (light)
	{
		for (int i = 0; i < m_LightsVec.size(); ++i)
		{
			if (m_LightsVec[i] == light)
				return i;
		}
	}

	return -1;
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Buffers ------------------------------------------------------
// ---------------------------------------------------------------------------------------------
uint ModuleRenderer3D::CreateBufferFromData(uint Targetbuffer, uint size, void* data) const
{
	uint ID = 0;

	glGenBuffers(1, (GLuint*)&ID); // create buffer
	glBindBuffer(Targetbuffer, ID); // start using created buffer
	glBufferData(Targetbuffer, size, data, GL_STATIC_DRAW); // send data to VRAM
	glBindBuffer(Targetbuffer, 0); // Stop using buffer

	return ID;
}

void ModuleRenderer3D::CreateFramebuffer()
{
	// --- Create a texture to use it as depthMap for shadowing ---
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, App->window->GetWindowWidth(), App->window->GetWindowHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &depthbufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthbufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// --- Create a texture to use it as render target ---
	glGenTextures(1, &rendertexture);
	glBindTexture(GL_TEXTURE_2D, rendertexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glBindTexture(GL_TEXTURE_2D, 0);

	// --- Generate attachments, DEPTH and STENCIL ---
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glBindTexture(GL_TEXTURE_2D, 0);

	// --- Generate framebuffer object (fbo) ---
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Utilities ----------------------------------------------------
// ---------------------------------------------------------------------------------------------
void ModuleRenderer3D::HandleObjectOutlining()
{
	// --- Selected Object Outlining ---
	for (GameObject* obj : *App->selection->GetSelected())
	{
		// --- Draw slightly scaled-up versions of the objects, disable stencil writing
		// The stencil buffer is filled with several 1s. The parts that are 1 are not drawn, only the objects size
		// differences, making it look like borders ---
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		// --- Search for Renderer Component ---
		ComponentMeshRenderer* MeshRenderer = obj->GetComponent<ComponentMeshRenderer>();

		// --- If Found, draw the mesh ---
		if (MeshRenderer && MeshRenderer->IsEnabled() && obj->GetActive())
		{
			std::vector<RenderMesh> meshInstances;

			ComponentMesh* cmesh = obj->GetComponent<ComponentMesh>();
			ComponentMeshRenderer* cmesh_renderer = obj->GetComponent<ComponentMeshRenderer>();
			RenderMeshFlags flags = outline;

			if (cmesh && cmesh->resource_mesh && cmesh_renderer && cmesh_renderer->material)
			{
				meshInstances.push_back(RenderMesh(obj->GetComponent<ComponentTransform>()->GetGlobalTransform(), cmesh->resource_mesh, cmesh_renderer->material, flags));
				DrawRenderMesh(meshInstances);
			}
		}
		//MeshRenderer->Draw(true);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}


void ModuleRenderer3D::UpdateGLCapabilities() const
{
	// --- Enable/Disable OpenGL Capabilities ---

	if (!depth)
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);

	if (!cull_face)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	if (!lighting)
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);

	if (!color_material)
		glDisable(GL_COLOR_MATERIAL);
	else
		glEnable(GL_COLOR_MATERIAL);

}

void ModuleRenderer3D::CreateGrid(float target_distance)
{
	// --- Fill vertex data ---

	float distance = target_distance / 4;

	if (distance < 1)
		distance = 1;

	float3 vertices[164];

	uint i = 0;
	int lines = -20;

	for (i = 0; i < 40; i++)
	{
		vertices[4 * i] = float3(lines * -distance, 0.0f, 20 * -distance);
		vertices[4 * i + 1] = float3(lines * -distance, 0.0f, 20 * distance);
		vertices[4 * i + 2] = float3(20 * -distance, 0.0f, lines * distance);
		vertices[4 * i + 3] = float3(20 * distance, 0.0f, lines * distance);

		lines++;
	}

	vertices[4 * i] = float3(lines * -distance, 0.0f, 20 * -distance);
	vertices[4 * i + 1] = float3(lines * -distance, 0.0f, 20 * distance);
	vertices[4 * i + 2] = float3(20 * -distance, 0.0f, lines * distance);
	vertices[4 * i + 3] = float3(20 * distance, 0.0f, lines * distance);

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


// ---------------------------------------------------------------------------------------------
// ------------------------------ Shaders ------------------------------------------------------
// ---------------------------------------------------------------------------------------------
void ModuleRenderer3D::CreateDefaultShaders()
{
	ImporterShader* IShader = App->resources->GetImporter<ImporterShader>();

	const char* vertexShaderT =
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 a_Position; \n"
		"layout(location = 1) in vec3 a_Normal; \n"
		"layout(location = 2) in vec3 a_Color; \n"
		"layout (location = 3) in vec2 a_TexCoord; \n"
		"uniform vec3 u_Color; \n"
		"uniform mat4 u_Model; \n"
		"uniform mat4 u_View; \n"
		"uniform mat4 u_Proj; \n"
		"out vec3 v_Color; \n"
		"out vec2 v_TexCoord; \n"
		"void main(){ \n"
		"gl_Position = u_Proj * u_View * u_Model * vec4 (a_Position, 1.0f); \n"
		"v_Color = u_Color; \n"
		"v_TexCoord = a_TexCoord; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* fragmentShaderT =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"uniform int u_UseTextures;\n"
		"in vec3 v_Color; \n"
		"in vec2 v_TexCoord; \n"
		"out vec4 color; \n"
		"uniform sampler2D u_AlbedoTexture; \n"
		"void main(){ \n"
		"color = texture(u_AlbedoTexture, v_TexCoord); \n"
		"if(u_UseTextures == -1)\n"
		"color = vec4(v_Color, 1);\n"
		"} \n"
		"#endif //FRAGMENT_SHADER\n"
		;

	VertexShaderTemplate = vertexShaderT;
	FragmentShaderTemplate = fragmentShaderT;


	// --- Creating outline drawing shaders ---
	const char* OutlineVertShaderSrc =
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 a_Position; \n"
		"uniform mat4 u_Model; \n"
		"uniform mat4 u_View; \n"
		"uniform mat4 u_Proj; \n"
		"void main(){ \n"
		"gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f); \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* OutlineFragShaderSrc =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(1.0,0.65,0.0, 1.0); \n"
		"} \n"
		"#endif //FRAGMENT_SHADER \n"
		;

	OutlineShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/OutlineShader.glsl", 8);
	OutlineShader->vShaderCode = OutlineVertShaderSrc;
	OutlineShader->fShaderCode = OutlineFragShaderSrc;
	OutlineShader->ReloadAndCompileShader();
	OutlineShader->SetName("OutlineShader");
	OutlineShader->LoadToMemory();
	IShader->Save(OutlineShader);

	// --- Creating point/line drawing shaders ---

	const char* linePointVertShaderSrc =
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 a_Position; \n"
		"out vec3 v_Color; \n"
		"uniform vec3 u_Color; \n"
		"uniform mat4 u_Model; \n"
		"uniform mat4 u_View; \n"
		"uniform mat4 u_Proj; \n"
		"void main(){ \n"
		"gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f); \n"
		"v_Color = u_Color; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* linePointFragShaderSrc =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"in vec3 v_Color; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(v_Color, 1.0); \n"
		"} \n"
		"#endif //FRAGMENT_SHADER\n"
		;

	linepointShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/LinePoint.glsl", 9);
	linepointShader->vShaderCode = linePointVertShaderSrc;
	linepointShader->fShaderCode = linePointFragShaderSrc;
	linepointShader->ReloadAndCompileShader();
	linepointShader->SetName("LinePoint");
	linepointShader->LoadToMemory();
	IShader->Save(linepointShader);


	// --- Creating z buffer shader drawer ---

	const char* zdrawervertex =
		R"(#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec3 a_Position;

		uniform vec2 nearfar;
		uniform mat4 u_Model;
		uniform mat4 u_View;
		uniform mat4 u_Proj;

		out mat4 v_Projection;
		out vec2 v_NearFarPlanes;

		void main()
		{
			v_NearFarPlanes = nearfar;
			v_Projection = u_Proj;
			gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
		}
		#endif //VERTEX_SHADER)";

	const char* zdrawerfragment =
		R"(#version 440 core
		#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

			in vec2 v_NearFarPlanes;
			in mat4 v_Projection;

			out vec4 color;

			float LinearizeDepth(float depth)
			{
				float z = 2.0 * depth - 1.0; // back to NDC
				return 2.0 * v_NearFarPlanes.x * v_NearFarPlanes.y / (v_NearFarPlanes.y + v_NearFarPlanes.x - z * (v_NearFarPlanes.y - v_NearFarPlanes.x));
			}

			void main()
			{
				float depth = LinearizeDepth(gl_FragCoord.z) / v_NearFarPlanes.y;
				color = vec4(vec3(gl_FragCoord.z * v_NearFarPlanes.y * v_NearFarPlanes.x), 1.0);
			}

		#endif //FRAGMENT_SHADER)";

	// NOTE: not removing linearizedepth function because it was needed for the previous z buffer implementation (no reversed-z), just in case I need it again (doubt it though)

	ZDrawerShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/ZDrawer.glsl", 10);
	ZDrawerShader->vShaderCode = zdrawervertex;
	ZDrawerShader->fShaderCode = zdrawerfragment;
	ZDrawerShader->ReloadAndCompileShader();
	ZDrawerShader->SetName("ZDrawer");
	ZDrawerShader->LoadToMemory();
	IShader->Save(ZDrawerShader);


	// --- Creating text rendering shaders ---

	const char* textVertShaderSrc =
		R"(#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec3 a_Position;
		layout (location = 1) in vec2 a_TexCoords;

		uniform mat4 u_Model;
		uniform mat4 u_View;
		uniform mat4 u_Proj;

		out vec2 v_TexCoords;

		void main()
		{
			gl_Position = u_Proj * u_View * u_Model * vec4 (a_Position, 1.0f);
			v_TexCoords = a_TexCoords;
		}

		#endif //VERTEX_SHADER)";

	const char* textFragShaderSrc =
		R"(#version 440 core
		#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		in vec2 v_TexCoords;

		uniform sampler2D text;
		uniform vec3 textColor;

		out vec4 color;

		void main()
		{
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, v_TexCoords).r);
			color = vec4(textColor, 1.0) * sampled;
		}

		#endif //FRAGMENT_SHADER)";

	textShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/TextShader.glsl", 11);
	textShader->vShaderCode = textVertShaderSrc;
	textShader->fShaderCode = textFragShaderSrc;
	textShader->ReloadAndCompileShader();
	textShader->SetName("TextShader");
	textShader->LoadToMemory();
	IShader->Save(textShader);


	// --- Creating Default Vertex and Fragment Shaders ---

	const char* vertexShaderSource =
		R"(#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec3 a_Position;
		layout(location = 1) in vec3 a_Normal;
		layout(location = 2) in vec3 a_Color;
		layout (location = 3) in vec2 a_TexCoord;

		uniform vec3 u_Color = vec3(1.0);
		uniform mat4 u_Model;
		uniform mat4 u_View;
		uniform mat4 u_Proj;

		out vec3 v_Color;
		out vec2 v_TexCoord;

		void main()
		{
			gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f);
			v_Color = u_Color;
			v_TexCoord = a_TexCoord;
		}
		#endif //VERTEX_SHADER)";

	const char* fragmentShaderSource =
		R"(#version 440 core
		#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		uniform int u_UseTextures;
		uniform sampler2D u_AlbedoTexture;

		in vec3 v_Color;
		in vec2 v_TexCoord;
		out vec4 color;

		void main()
		{
			color = texture(u_AlbedoTexture, v_TexCoord) * vec4(v_Color, 1);
			if(u_UseTextures == -1)
				color = vec4(v_Color, 1);
		}
		#endif //FRAGMENT_SHADER)";

	defaultShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/Standard.glsl", 12);
	defaultShader->vShaderCode = vertexShaderSource;
	defaultShader->fShaderCode = fragmentShaderSource;
	defaultShader->ReloadAndCompileShader();
	defaultShader->SetName("Standard");
	defaultShader->LoadToMemory();
	defaultShader->ReloadAndCompileShader();
	IShader->Save(defaultShader);

	// ---Creating screen shader ---

	const char* vertexScreenShader =
		R"(#version 440 core
		#define VERTEX_SHADER
		#ifdef VERTEX_SHADER

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoords;

		out vec2 TexCoords;

		void main()
		{
			gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
			TexCoords = aTexCoords;
		}
		#endif //VERTEX_SHADER)";

	const char* fragmentScreenShader =
		R"(#version 440 core
		#define FRAGMENT_SHADER
		#ifdef FRAGMENT_SHADER

		out vec4 FragColor;

		in vec2 TexCoords;

		uniform sampler2D screenTexture;

		void main()
		{
		    FragColor = texture(screenTexture, TexCoords);
		}
		#endif //FRAGMENT_SHADER)";

	screenShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/ScreenShader.glsl", 13);
	screenShader->vShaderCode = vertexScreenShader;
	screenShader->fShaderCode = fragmentScreenShader;
	screenShader->ReloadAndCompileShader();
	screenShader->SetName("Screen shader");
	screenShader->LoadToMemory();
	screenShader->ReloadAndCompileShader();
	IShader->Save(screenShader);

	defaultShader->use();
}


// ---------------------------------------------------------------------------------------------
// ------------------------------ Draw Utilities -----------------------------------------------
// ---------------------------------------------------------------------------------------------
void ModuleRenderer3D::DrawRenderLines()
{
	// --- Use linepoint shader ---
	glUseProgram(App->renderer3D->linepointShader->ID);

	// --- Get Uniform locations ---
	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Model");
	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_View");
	int vertexColorLocation = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Color");
	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Proj");

	// --- Set Right handed projection matrix ---

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	// --- Set Uniforms ---
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	// --- Initialize vars, prepare buffer ---
	float3* vertices = new float3[2];
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(PointLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// --- Draw Lines ---
	for (std::vector<RenderLine>::const_iterator it = render_lines.begin(); it != render_lines.end(); ++it)
	{
		// --- Assign color and model matrix ---
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (*it).transform.Transposed().ptr());
		glUniform3f(vertexColorLocation, (*it).color.r / 255.0f, (*it).color.g / 255.0f, (*it).color.b / 255.0f);

		// --- Assign line vertices, a and b ---
		vertices[0] = (*it).a;
		vertices[1] = (*it).b;

		// --- Send data ---
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 2, vertices, GL_DYNAMIC_DRAW);

		// --- Draw lines ---
		glLineWidth(3.0f);
		glBindVertexArray(PointLineVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
		glLineWidth(1.0f);
	}

	// --- Reset stuff ---
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// --- Delete VBO and vertices ---
	glDeleteBuffers(1, &VBO);
	delete[] vertices;

	// --- Back to default ---
	glUseProgram(App->renderer3D->defaultShader->ID);
}

void ModuleRenderer3D::DrawRenderBoxes()
{
	for (uint i = 0; i < render_obbs.size(); ++i)
	{
		DrawWire(*render_obbs[i].box, render_obbs[i].color, PointLineVAO);
	}
	for (uint i = 0; i < render_aabbs.size(); ++i)
	{
		DrawWire(*render_aabbs[i].box, render_aabbs[i].color, PointLineVAO);
	}

	for (uint i = 0; i < render_frustums.size(); ++i)
	{
		DrawWire(*render_frustums[i].box, render_frustums[i].color, PointLineVAO);
	}
}

void ModuleRenderer3D::DrawGrid()
{
	//App->renderer3D->defaultShader->use();
	uint shaderID = App->renderer3D->defaultShader->ID;
	glUseProgram(shaderID);

	GLint modelLoc = glGetUniformLocation(shaderID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

	float gridColor = 0.8f;
	GLint vertexColorLocation = glGetUniformLocation(shaderID, "u_Color");
	glUniform3f(vertexColorLocation, gridColor, gridColor, gridColor);

	glUniform1i(glGetUniformLocation(shaderID, "u_UseTextures"), 0);

	glLineWidth(1.7f);
	glBindVertexArray(Grid_VAO);
	glDrawArrays(GL_LINES, 0, 164);
	glBindVertexArray(0);
	glLineWidth(1.0f);

	glUseProgram(0);
}

void ModuleRenderer3D::DrawWireFromVertices(const float3* corners, Color color, uint VAO) {
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
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	int vertexColorLocation = glGetUniformLocation(App->renderer3D->linepointShader->ID, "u_Color");
	glUniform3f(vertexColorLocation, color.r, color.g, color.b);

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
