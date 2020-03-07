#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMeshRenderer.h"
#include "ResourceShader.h"

#include "PanelScene.h"

#include "Imgui/imgui.h"
#include "OpenGL.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment(lib, "glew/libx86/glew32.lib")

#include "mmgr/mmgr.h"

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(json file)
{
	CONSOLE_LOG("Creating 3D Renderer context");

	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if(context == NULL)
	{
		CONSOLE_LOG("|[error]: OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(vsync && SDL_GL_SetSwapInterval(1) < 0)
			CONSOLE_LOG("|[error]: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		// Initialize glew
		GLenum error = glewInit();

		if (error != GL_NO_ERROR)
		{
			CONSOLE_LOG("|[error]: Error initializing glew! %s\n"/*, glewGetErrorString(error)*/);
			ret = false;
		}

	}

	// --- z values from 0 to 1 and not -1 to 1, more precision in far ranges --- 
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	// --- Enable stencil testing, set to replace ---
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

	// --- Multitexturing ---
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, (GLint*)&maxSimultaneousTextures);

	for (uint i = 0; i < maxSimultaneousTextures; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glEnable(GL_TEXTURE_2D);
	}

	glActiveTexture(GL_TEXTURE0);

	// --- Check if graphics driver supports shaders in binary format ---
	//GLint formats = 0;
	//glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	//if (formats < 1) {
	//	std::cerr << "Driver does not support any binary formats." << std::endl;
	//	exit(EXIT_FAILURE);
	//}

	CONSOLE_LOG("OpenGL Version: %s", glGetString(GL_VERSION));
	CONSOLE_LOG("Glew Version: %s", glewGetString(GLEW_VERSION));

	// --- Creating outline drawing shaders ---
	const char* OutlineVertShaderSrc = "#version 460 core \n"
		"layout (location = 0) in vec3 position; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"}\n";

	const char* OutlineFragShaderSrc = "#version 460 core \n"
		"in vec3 ourColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(1.0,0.65,0.0, 1.0); \n"
		"} \n";

	OutlineShader = new ResourceShader(OutlineVertShaderSrc, OutlineFragShaderSrc, false);
	OutlineShader->name = "OutlineShader";

	// --- Creating point/line drawing shaders ---

	const char * linePointVertShaderSrc = "#version 460 core \n"
		"layout (location = 0) in vec3 position; \n"
		"out vec3 ourColor; \n"
		"in vec3 color; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"ourColor = color; \n"
		"}\n";

	const char * linePointFragShaderSrc = "#version 460 core \n"
		"in vec3 ourColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(ourColor, 1.0); \n"
		"} \n";

	linepointShader = new ResourceShader(linePointVertShaderSrc, linePointFragShaderSrc, false);
	linepointShader->name = "LinePoint";

	// --- Creating z buffer shader drawer ---

	const char * zdrawervertex = "#version 460 core \n"
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
		"}\n";

	const char * zdrawerfragment = "#version 460 core \n"
		"out vec4 FragColor; \n"
		"in vec2 nearfarfrag; \n"
		"in mat4 _projection; \n"
		"float LinearizeDepth(float depth){ \n"
		"float z =  2.0*depth - 1.0; // back to NDC \n"
		"return 2.0* nearfarfrag.x * nearfarfrag.y / (nearfarfrag.y + nearfarfrag.x - z * (nearfarfrag.y - nearfarfrag.x)); }\n"
		"void main(){ \n"
		"float depth = LinearizeDepth(gl_FragCoord.z) / nearfarfrag.y;  \n"
		"FragColor = vec4(vec3(gl_FragCoord.z*nearfarfrag.y*nearfarfrag.x), 1.0); } \n";
	// NOTE: not removing linearizedepth function because it was needed for the previous z buffer implementation (no reversed-z)

	ZDrawerShader = new ResourceShader(zdrawervertex, zdrawerfragment, false);
	ZDrawerShader->name = "ZDrawer";

	// --- Creating Default Vertex and Fragment Shaders ---

	const char *vertexShaderSource =
		"#version 460 core \n"
		"layout (location = 0) in vec3 position; \n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec3 color; \n"
		"layout (location = 3) in vec2 texCoord; \n"
		"out vec3 ourColor; \n"
		"out vec2 TexCoord; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"ourColor = color; \n"
		"TexCoord = texCoord; \n"
		"}\n"
		;

	const char *fragmentShaderSource =
		"#version 460 core \n"
		"in vec3 ourColor; \n"
		"in vec2 TexCoord; \n"
		"out vec4 color; \n"
		"uniform sampler2D ourTexture; \n"
		"void main(){ \n"
		"color = texture(ourTexture, TexCoord); \n"
		"} \n"
		;

	defaultShader = new ResourceShader(vertexShaderSource, fragmentShaderSource, false);
	defaultShader->name = "Standard";
	defaultShader->use();

	// --- Set engine's basic shaders ---
	CreateDefaultShaders();

	//Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

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
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glClearDepth(0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	// --- Set Shader Matrices ---
	GLint viewLoc = glGetUniformLocation(defaultShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
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
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glDepthFunc(GL_GREATER);

	// --- Do not write to the stencil buffer ---
	glStencilMask(0x00);

	// --- Draw Level Geometry ---
	App->scene_manager->Draw();

	// --- Selected Object Outlining ---
	HandleObjectOutlining();

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
		if (MeshRenderer && MeshRenderer->IsEnabled())
			MeshRenderer->Draw(true);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	// --- Back to defaults ---
	glDepthFunc(GL_LESS);

	// --- Unbind fbo ---
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- Draw ui and swap buffers ---
	App->gui->Draw();

	// --- To prevent problems with viewports, disabled due to crashes and conflicts with docking, sets a window as current rendering context ---
	SDL_GL_MakeCurrent(App->window->window, context); 
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_LOG("Destroying 3D Renderer");

	delete defaultShader;
	delete linepointShader;
	delete ZDrawerShader;
	delete OutlineShader;

	glDeleteFramebuffers(1, &fbo);
	SDL_GL_DeleteContext(context);

	return true;
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


void ModuleRenderer3D::OnResize(int width, int height)
{
	// --- Called by UpdateWindowSize() in Window module this when resizing windows to prevent rendering issues ---

	// --- Resetting View matrices ---
	glViewport(0, 0, width, height);

	if(width > height)
		active_camera->SetAspectRatio(width / height);
	else
		active_camera->SetAspectRatio(height / width);

	glDeleteFramebuffers(1, &fbo);
	CreateFramebuffer();
}

uint ModuleRenderer3D::CreateBufferFromData(uint Targetbuffer, uint size, void * data) const
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

bool ModuleRenderer3D::SetVSync(bool vsync)
{
	bool ret = true;

	this->vsync = vsync;

	if (this->vsync)
	{

		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			ret = false;
			CONSOLE_LOG("|[error]: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
	}
	else {

		if (SDL_GL_SetSwapInterval(0) == -1)
		{
			ret = false;
			CONSOLE_LOG("|[error]: Warning: Unable to set immediate updates! SDL Error: %s\n", SDL_GetError());
		}
	}

	return ret;
}

void ModuleRenderer3D::SetActiveCamera(ComponentCamera * camera)
{
	if (this->active_camera)
	{
		this->active_camera->active_camera = false;
	}
	// if camera is not nullptr, then we set it as active camera, else we set editor camera as active camera
	this->active_camera = camera ? camera : App->camera->camera;
	if (camera)
	{
		camera->active_camera = true;
	}
}

void ModuleRenderer3D::SetCullingCamera(ComponentCamera * camera)
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

bool ModuleRenderer3D::GetVSync() const
{
	return vsync;
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
			MeshRenderer->Draw(true);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void ModuleRenderer3D::CreateDefaultShaders()
{
	// --- Creating outline drawing shaders ---
	const char* OutlineVertShaderSrc = "#version 440 core \n"
		"layout (location = 0) in vec3 position; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"}\n";

	const char* OutlineFragShaderSrc = "#version 440 core \n"
		"in vec3 ourColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(1.0,0.65,0.0, 1.0); \n"
		"} \n";

	OutlineShader = new ResourceShader(OutlineVertShaderSrc, OutlineFragShaderSrc, false);
	OutlineShader->name = "OutlineShader";

	// --- Creating point/line drawing shaders ---

	const char* linePointVertShaderSrc = "#version 440 core \n"
		"layout (location = 0) in vec3 position; \n"
		"out vec3 ourColor; \n"
		"uniform vec3 Color; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f); \n"
		"ourColor = Color; \n"
		"}\n";

	const char* linePointFragShaderSrc = "#version 440 core \n"
		"in vec3 ourColor; \n"
		"out vec4 color; \n"
		"void main(){ \n"
		"color = vec4(ourColor, 1.0); \n"
		"} \n";

	linepointShader = new ResourceShader(linePointVertShaderSrc, linePointFragShaderSrc, false);
	linepointShader->name = "LinePoint";

	// --- Creating z buffer shader drawer ---

	const char* zdrawervertex = "#version 440 core \n"
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
		"}\n";

	const char* zdrawerfragment = "#version 440 core \n"
		"out vec4 FragColor; \n"
		"in vec2 nearfarfrag; \n"
		"in mat4 _projection; \n"
		"float LinearizeDepth(float depth){ \n"
		"float z =  2.0*depth - 1.0; // back to NDC \n"
		"return 2.0* nearfarfrag.x * nearfarfrag.y / (nearfarfrag.y + nearfarfrag.x - z * (nearfarfrag.y - nearfarfrag.x)); }\n"
		"void main(){ \n"
		"float depth = LinearizeDepth(gl_FragCoord.z) / nearfarfrag.y;  \n"
		"FragColor = vec4(vec3(gl_FragCoord.z*nearfarfrag.y*nearfarfrag.x), 1.0); } \n";
	// NOTE: not removing linearizedepth function because it was needed for the previous z buffer implementation (no reversed-z), just in case I need it again (doubt it though)

	ZDrawerShader = new ResourceShader(zdrawervertex, zdrawerfragment, false);
	ZDrawerShader->name = "ZDrawer";

	// --- Creating Default Vertex and Fragment Shaders ---

	const char* vertexShaderSource =
		"#version 440 core \n"
		"layout (location = 0) in vec3 position; \n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec3 color; \n"
		"layout (location = 3) in vec2 texCoord; \n"
		"layout (location = 4) in vec3 animPos_offset; \n"
		"uniform vec3 Color; \n"
		"out vec3 ourColor; \n"
		"out vec2 TexCoord; \n"
		"uniform mat4 model_matrix; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main(){ \n"
		"vec3 final_pos = animPos_offset;\n"
		"if(animPos_offset.x == 0 && animPos_offset.y == 0 && animPos_offset.z == 0){\n"
		"final_pos = position; \n"
		"}\n"
		"gl_Position = projection * view * model_matrix * vec4 (final_pos, 1.0f); \n"
		"ourColor = Color; \n"
		"TexCoord = texCoord; \n"
		"}\n"
		;

	const char* fragmentShaderSource =
		"#version 440 core \n"
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
		;

	defaultShader = new ResourceShader(vertexShaderSource, fragmentShaderSource, false);
	defaultShader->name = "Standard";
	defaultShader->use();
}
