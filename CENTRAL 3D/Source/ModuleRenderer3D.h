#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "JSONLoader.h"

#define MAX_LIGHTS 8

class ComponentCamera;
class ResourceShader;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(json file) override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	// --- Utilities ---
	void UpdateGLCapabilities() const;
	void OnResize(int width, int height);

	uint CreateBufferFromData(uint Targetbuffer, uint size, void* data) const;
	void CreateFramebuffer();

	// --- Setters ---
	bool SetVSync(bool vsync);
	void SetActiveCamera(ComponentCamera* camera);
	void SetCullingCamera(ComponentCamera* camera);

	// --- Getters ---
	bool GetVSync() const;

private:
	void HandleObjectOutlining();
	void CreateDefaultShaders();

	uint maxSimultaneousTextures = 0;

public:
	// --- Default Shader ---
	ResourceShader* defaultShader = nullptr;
	ResourceShader* linepointShader = nullptr;
	ResourceShader* OutlineShader = nullptr;
	ResourceShader* ZDrawerShader = nullptr;

	unsigned int VAO = 0;
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	ComponentCamera* active_camera = nullptr;
	ComponentCamera* culling_camera = nullptr;

	uint fbo = 0;
	uint depthbuffer = 0;
	uint rendertexture = 0;

	// --- Flags ---
	bool vsync = true;
	bool depth = true;
	bool cull_face = true;
	bool lighting = false;
	bool color_material = true;
	bool wireframe = false;
	bool zdrawer = false;
};