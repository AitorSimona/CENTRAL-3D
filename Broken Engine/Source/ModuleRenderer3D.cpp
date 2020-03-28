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
#include "ComponentAudioListener.h"
#include "Component.h"

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

// ------------------------------ Basic --------------------------------------------------------

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled) {
	name = "Renderer3D";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D() {
}

// Called before render is available
bool ModuleRenderer3D::Init(json& file) {
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Creating 3D Renderer context");

	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if (context == NULL) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true) {
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
			GL_SETERRORHANDLER(4, 4); //OpenGL Error Handler
	}

	// --- z values from 0 to 1 and not -1 to 1, more precision in far ranges ---
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	// --- Enable stencil testing, set to replace ---
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

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	// --- Clear render orders ---
	ClearRenderOrders();

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
update_status ModuleRenderer3D::PostUpdate(float dt) {
	// --- Set Shader Matrices ---
	GLint viewLoc = glGetUniformLocation(defaultShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix (just different standard)
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(defaultShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	GLint modelLoc = glGetUniformLocation(defaultShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.Transposed().ptr());

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
	DrawRenderMeshes();
	DrawRenderLines();
	DrawRenderBoxes();
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

	// --- Draw GUI and swap buffers ---
	App->gui->Draw();

	// --- To prevent problems with viewports, disabled due to crashes and conflicts with docking, sets a window as current rendering context ---
	SDL_GL_MakeCurrent(App->window->window, context);
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Destroying 3D Renderer");

	glDeleteBuffers(1, (GLuint*)&Grid_VBO);
	glDeleteVertexArrays(1, &Grid_VAO);

	glDeleteFramebuffers(1, &fbo);
	SDL_GL_DeleteContext(context);

	return true;
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

// ----------------------------------------------------


// ------------------------------ Setters --------------------------------------------------------


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

// ----------------------------------------------------


// ------------------------------ Getters --------------------------------------------------------

bool ModuleRenderer3D::GetVSync() const
{
	return vsync;
}

// ----------------------------------------------------


// ------------------------------ Render Orders --------------------------------------------------------

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

			render_meshes[mesh->GetUID()].push_back(rmesh);
		}
		else
		{
			// --- Build new vector to store mesh's instances ---
			std::vector<RenderMesh> new_vec;

			RenderMesh rmesh = RenderMesh(transform, mesh, mat, flags/*, color*/);
			rmesh.deformable_mesh = deformable_mesh; // TEMPORAL!

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

void ModuleRenderer3D::DrawFrustum(const Frustum& box, const Color& color)
{
	if (box.IsFinite())
		render_frustums.push_back(RenderBox<Frustum>(&box, color));
}

// ----------------------------------------------------


// ------------------------------ Utilities --------------------------------------------------------

void ModuleRenderer3D::ClearRenderOrders()
{
	render_meshes.clear();
	render_aabbs.clear();
	render_frustums.clear();
	render_lines.clear();
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

void ModuleRenderer3D::CreateDefaultShaders()
{
	ImporterShader* IShader = App->resources->GetImporter<ImporterShader>();

	const char* vertexShaderT =
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 position; \n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec3 color; \n"
		"layout (location = 3) in vec2 texCoord; \n"
		"uniform vec3 Color; \n"
		"out vec3 ourColor; \n"
		"out vec2 TexCoord; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4 (position, 1.0f); \n"
		"ourColor = Color; \n"
		"TexCoord = texCoord; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* fragmentShaderT =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"uniform int Texture;\n"
		"in vec3 ourColor; \n"
		"in vec2 TexCoord; \n"
		"out vec4 color; \n"
		"uniform sampler2D ourTexture; \n"
		"void main(){ \n"
		"color = texture(ourTexture, TexCoord); \n"
		"if(Texture == -1)\n"
		"color = vec4(ourColor, 1);\n"
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
		"layout (location = 0) in vec3 position; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* OutlineFragShaderSrc =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"in vec3 ourColor; \n"
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
		"layout (location = 0) in vec3 position; \n"
		"out vec3 ourColor; \n"
		"uniform vec3 Color; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"ourColor = Color; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* linePointFragShaderSrc =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"in vec3 ourColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(ourColor, 1.0); \n"
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
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 position; \n"
		"uniform vec2 nearfar; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"out mat4 _projection; \n"
		"out vec2 nearfarfrag; \n"
		"void main(){ \n"
		"nearfarfrag = nearfar; \n"
		"_projection = projection; \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* zdrawerfragment =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"out vec4 FragColor; \n"
		"in vec2 nearfarfrag; \n"
		"in mat4 _projection; \n"
		"float LinearizeDepth(float depth){ \n"
		"float z =  2.0*depth - 1.0; // back to NDC \n"
		"return 2.0* nearfarfrag.x * nearfarfrag.y / (nearfarfrag.y + nearfarfrag.x - z * (nearfarfrag.y - nearfarfrag.x)); }\n"
		"void main(){ \n"
		"float depth = LinearizeDepth(gl_FragCoord.z) / nearfarfrag.y;  \n"
		"FragColor = vec4(vec3(gl_FragCoord.z*nearfarfrag.y*nearfarfrag.x), 1.0); } \n"
		"#endif //FRAGMENT_SHADER\n"
		;
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
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 position; \n"
		"layout (location = 1) in vec2 texCoords; \n"
		"out vec2 TexCoords; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4 (position, 1.0f); \n"
		"TexCoords = texCoords; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* textFragShaderSrc =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"in vec2 TexCoords; \n"
		"uniform sampler2D text; \n"
		"uniform vec3 textColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); \n"
		"color = vec4(textColor, 1.0) * sampled; \n"
		"} \n"
		"#endif //FRAGMENT_SHADER\n"
		;

	textShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/TextShader.glsl", 11);
	textShader->vShaderCode = textVertShaderSrc;
	textShader->fShaderCode = textFragShaderSrc;
	textShader->ReloadAndCompileShader();
	textShader->SetName("TextShader");
	textShader->LoadToMemory();
	IShader->Save(textShader);


	// --- Creating Default Vertex and Fragment Shaders ---

	const char* vertexShaderSource =
		"#version 440 core \n"
		"#define VERTEX_SHADER \n"
		"#ifdef VERTEX_SHADER \n"
		"layout (location = 0) in vec3 position; \n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec3 color; \n"
		"layout (location = 3) in vec2 texCoord; \n"
		"uniform vec3 Color = vec3(1.0); \n"
		"out vec3 ourColor; \n"
		"out vec2 TexCoord; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4 (position, 1.0f); \n"
		"ourColor = Color; \n"
		"TexCoord = texCoord; \n"
		"}\n"
		"#endif //VERTEX_SHADER\n"
		;

	const char* fragmentShaderSource =
		"#version 440 core \n"
		"#define FRAGMENT_SHADER \n"
		"#ifdef FRAGMENT_SHADER \n"
		"uniform int Texture;\n"
		"in vec3 ourColor; \n"
		"in vec2 TexCoord; \n"
		"out vec4 color; \n"
		"uniform sampler2D ourTexture; \n"
		"void main(){ \n"
		"color = texture(ourTexture, TexCoord) * vec4(ourColor, 1); \n"
		"if(Texture == -1)\n"
		"color = vec4(ourColor, 1);\n"
		"} \n"
		"#endif //FRAGMENT_SHADER\n"
		;

	defaultShader = (ResourceShader*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SHADER, "Assets/Shaders/Standard.glsl", 12);
	defaultShader->vShaderCode = vertexShaderSource;
	defaultShader->fShaderCode = fragmentShaderSource;
	defaultShader->ReloadAndCompileShader();
	defaultShader->SetName("Standard");
	defaultShader->LoadToMemory();
	IShader->Save(defaultShader);

	defaultShader->use();
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


// ----------------------------------------------------


// ------------------------------ Draw --------------------------------------------------------


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
	for(uint i = 0; i < meshInstances.size(); ++i)
	{
		RenderMesh* mesh = &meshInstances[i];
		uint shader = defaultShader->ID;
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
			// --- Draw selected, pass scaled-up matrix to shader ---
			float3 scale = float3(1.05f, 1.05f, 1.05f);

			model = float4x4::FromTRS(model.TranslatePart(), model.RotatePart(), scale);
		}

		// --- Display Z buffer ---
		if (zdrawer)
		{
			shader = ZDrawerShader->ID;
		}

		// --- Get Mesh Material ---
		if (mesh->mat->shader)
		{
			shader = mesh->mat->shader->ID;
			mesh->mat->UpdateUniforms();
		}

		glUseProgram(shader);

		// --- Set uniforms ---
		GLint modelLoc = glGetUniformLocation(shader, "model_matrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.Transposed().ptr()); // model matrix

		GLint viewLoc = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, active_camera->GetOpenGLViewMatrix().ptr());

		GLint timeLoc = glGetUniformLocation(shader, "time");
		glUniform1f(timeLoc, App->time->time);

		int TextureSupportLocation = glGetUniformLocation(shader, "Texture"); // as of now, this is only on DefaultShader!
		int vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");

		float farp = active_camera->GetFarPlane();
		float nearp = active_camera->GetNearPlane();

		// --- Give ZDrawer near and far camera frustum planes pos ---
		if (zdrawer)
		{
			int nearfarLoc = glGetUniformLocation(shader, "nearfar");
			glUniform2f(nearfarLoc, nearp, farp);
		}

		// right handed projection matrix
		float f = 1.0f / tan(active_camera->GetFOV() * DEGTORAD / 2.0f);
		float4x4 proj_RH(
			f / active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
			0.0f, f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, nearp, 0.0f);

		GLint projectLoc = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

		//Send Color
		glUniform3f(vertexColorLocation, colorToDraw.x, colorToDraw.y, colorToDraw.z);

		if (mesh->flags & RenderMeshFlags_::wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (mesh->resource_mesh->vertices && mesh->resource_mesh->Indices)
		{
			const ResourceMesh* rmesh = mesh->resource_mesh;

			if (mesh->deformable_mesh)
				rmesh = mesh->deformable_mesh;

			glBindVertexArray(rmesh->VAO);

			if (mesh->flags & RenderMeshFlags_::texture)
			{
				if (mesh->flags & RenderMeshFlags_::checkers)
					glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
				else
				{
					//glActiveTexture(GL_TEXTURE1);
					if (mesh->mat && mesh->mat->m_DiffuseResTexture)
					{
						glUniform3f(vertexColorLocation, mesh->mat->m_AmbientColor.x, mesh->mat->m_AmbientColor.y, mesh->mat->m_AmbientColor.z);
						glUniform1f(glGetUniformLocation(App->renderer3D->defaultShader->ID, "u_Shininess"), mesh->mat->m_Shininess);
						glUniform1i(TextureSupportLocation, (int)mesh->mat->m_UseTexture);

						glUniform1i(glGetUniformLocation(shader, "ourTexture"), 1);
						glActiveTexture(GL_TEXTURE0 + 1);
						glBindTexture(GL_TEXTURE_2D, mesh->mat->m_DiffuseResTexture->GetTexID());

						if (mesh->mat->m_SpecularResTexture)
						{
							glUniform1i(glGetUniformLocation(shader, "SpecText"), 2);
							glActiveTexture(GL_TEXTURE0 + 2);
							glBindTexture(GL_TEXTURE_2D, mesh->mat->m_SpecularResTexture->GetTexID());
						}
					}
					else
						glBindTexture(GL_TEXTURE_2D, App->textures->GetDefaultTextureID());
				}
			}
			else
				glUniform1i(TextureSupportLocation, (int)false);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->EBO);
			glDrawElements(GL_TRIANGLES, rmesh->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
		}

		// --- DeActivate wireframe mode ---
		if (mesh->flags & RenderMeshFlags_::wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (mesh->flags & RenderMeshFlags_::selected)
		{
			glStencilMask(0x00);
		}

		// --- Set uniforms back to defaults ---
		glUniform3f(vertexColorLocation, 255, 255, 255);
	}



	glUseProgram(defaultShader->ID);
}

void ModuleRenderer3D::HandleObjectOutlining()
{
	// --- Selected Object Outlining ---
	if (App->scene_manager->GetSelectedGameObject() != nullptr)
	{
		// --- Draw slightly scaled-up versions of the objects, disable stencil writing
		// The stencil buffer is filled with several 1s. The parts that are 1 are not drawn, only the objects size
		// differences, making it look like borders ---
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		// --- Search for Renderer Component ---
		ComponentMeshRenderer* MeshRenderer = App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentMeshRenderer>();

		// --- If Found, draw the mesh ---
		if (MeshRenderer && MeshRenderer->IsEnabled() && App->scene_manager->GetSelectedGameObject()->GetActive())
		{
			std::vector<RenderMesh> meshInstances;

			ComponentMesh* cmesh = App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentMesh>();
			ComponentMeshRenderer* cmesh_renderer = App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentMeshRenderer>();
			RenderMeshFlags flags = outline;

			if (cmesh && cmesh->resource_mesh && cmesh_renderer && cmesh_renderer->material)
			{
				meshInstances.push_back(RenderMesh(App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentTransform>()->GetGlobalTransform(), cmesh->resource_mesh, cmesh_renderer->material, flags));
				DrawRenderMesh(meshInstances);
			}
		}
		//MeshRenderer->Draw(true);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void ModuleRenderer3D::DrawRenderLines()
{
	// --- Use linepoint shader ---
	glUseProgram(App->renderer3D->linepointShader->ID);

	// --- Get Uniform locations ---
	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	int vertexColorLocation = glGetUniformLocation(App->renderer3D->linepointShader->ID, "Color");
	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");

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
	App->renderer3D->defaultShader->use();

	GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

	float gridColor = 0.8f;
	int vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
	glUniform3f(vertexColorLocation, gridColor, gridColor, gridColor);

	int TextureSupportLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
	glUniform1i(TextureSupportLocation, (int)false);

	glLineWidth(1.7f);
	glBindVertexArray(Grid_VAO);
	glDrawArrays(GL_LINES, 0, 164);
	glBindVertexArray(0);
	glLineWidth(1.0f);

	glUseProgram(0);
	glUniform1i(TextureSupportLocation, (int)false);
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

	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	int vertexColorLocation = glGetUniformLocation(App->renderer3D->linepointShader->ID, "Color");
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

// ----------------------------------------------------
