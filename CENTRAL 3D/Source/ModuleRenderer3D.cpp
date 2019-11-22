#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"

#include "ComponentCamera.h"

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


		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("|[error]: Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("|[error]: Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_LOG("|[error]: Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		// --- Set lights and OpenGL Capabilities ---
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);


		// Transparency and color merge
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	}

	CONSOLE_LOG("OpenGL Version: %s", glGetString(GL_VERSION));
	CONSOLE_LOG("Glew Version: %s", glewGetString(GLEW_VERSION));

	// Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	if (active_camera->update_projection)
	{
		UpdateProjectionMatrix();
		active_camera->update_projection = false;
	}

	// --- Reset Buffers to default values ---
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glLoadIdentity();

	// --- Set Model View as current ---
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(active_camera->GetOpenGLViewMatrix().ptr());

	// --- Update OpenGL Capabilities ---
	UpdateGLCapabilities();

	// light 0 on cam pos, Render lights
	lights[0].SetPos(active_camera->frustum.Pos().x, active_camera->frustum.Pos().y, active_camera->frustum.Pos().z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glClearColor(0.278f, 0.278f, 0.278f, 0.278f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);

	// --- Draw Level Geometry ---
	App->scene_manager->Draw();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.278f, 0.278f, 0.278f, 0.278f);

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

void ModuleRenderer3D::UpdateProjectionMatrix() const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((GLfloat*)active_camera->GetOpenGLProjectionMatrix().ptr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	// --- Called by UpdateWindowSize() in Window module this when resizing windows to prevent rendering issues ---

	// --- Resetting View matrices ---

	glViewport(0, 0, width, height);
	active_camera->SetAspectRatio(width / height);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	//glLoadMatrixf(&ProjectionMatrix);
	 UpdateProjectionMatrix();

	// glDeleteFramebuffers(1, &fbo);
	 //CreateFramebuffer();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
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

	UpdateProjectionMatrix();
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
