#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"
#include "ModuleResources.h"

#include "ComponentCamera.h"
#include "ResourceShader.h"

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

	CONSOLE_LOG("OpenGL Version: %s", glGetString(GL_VERSION));
	CONSOLE_LOG("Glew Version: %s", glewGetString(GLEW_VERSION));

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

	App->resources->AddShader(defaultShader);

	//Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	// --- MYTODO: Remove this
	if (active_camera->update_projection)
	{
		active_camera->update_projection = false;
	}
	// --- Update OpenGL Capabilities ---
	UpdateGLCapabilities();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	// --- Set Shader Matrices ---
	GLint viewLoc = glGetUniformLocation(defaultShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint projectLoc = glGetUniformLocation(defaultShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLProjectionMatrix().ptr());

	GLint modelLoc = glGetUniformLocation(defaultShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::identity.Transposed().ptr());

	// --- Clear framebuffers ---
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// --- Draw Level Geometry ---
	App->scene_manager->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- Draw everything and swap buffers ---
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
	// --- Generate framebuffer object (fbo) ---
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// --- Create a texture to use it as render target ---
	glGenTextures(1, &rendertexture);
	glBindTexture(GL_TEXTURE_2D, rendertexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->GetWindowWidth(), App->window->GetWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// --- Generate attachments, DEPTH and STENCIL render buffer objects ---
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_LOG("|[error]: Could not create framebuffer");

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
