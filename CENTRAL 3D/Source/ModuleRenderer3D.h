#ifndef __MODULE_RENDERER3D_H__
#define __MODULE_RENDERER3D_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "JSONLoader.h"

#define MAX_LIGHTS 8

class ComponentCamera;
class ResourceShader;
class ResourceMesh;
class ResourceMaterial;
class math::float4x4;
class GameObject;

typedef int RenderMeshFlags;

enum  RenderMeshFlags_
{
	None = 0,
	outline = 1 << 0,
	selected = 1 << 1,
	checkers = 1 << 2,
	wire = 1 << 3,
	texture = 1 << 4
};

struct  RenderMesh
{
	RenderMesh(float4x4 transform, const ResourceMesh* mesh, ResourceMaterial* mat, const RenderMeshFlags flags = 0) : transform(transform), resource_mesh(mesh), mat(mat), flags(flags){}

	float4x4 transform;
	const ResourceMesh* resource_mesh = nullptr;
	ResourceMaterial* mat = nullptr;
	//	Color color; // force a color draw, useful if no texture is given

	// --- Add rendering options here ---
	RenderMeshFlags flags = None;
};

template <typename Box>
struct  RenderBox
{
	RenderBox(const Box* box, const Color& color) : box(box), color(color) {}

	const Box* box;
	Color color;
};

struct  RenderLine
{
	RenderLine(float4x4 transform, const float3& a, const float3& b, const Color& color) : transform(transform), a(a), b(b), color(color) {}

	float4x4 transform;
	float3 a;
	float3 b;
	Color color;
};

class ModuleRenderer3D : public Module
{
	friend class ModuleResourceManager;
public:

	// --- Basic ---
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(json file) override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);

	// --- Setters ---
	bool SetVSync(bool _vsync);
	void SetActiveCamera(ComponentCamera* camera);
	void SetCullingCamera(ComponentCamera* camera);

	// --- Getters ---
	bool GetVSync() const;

	// --- Render orders --- // Deformable mesh is Temporal!
	void DrawMesh(const float4x4 transform, const ResourceMesh* mesh, ResourceMaterial* mat, const RenderMeshFlags flags = 0);
	void DrawLine(const float4x4 transform, const float3 a, const float3 b, const Color& color);
	void DrawAABB(const AABB& box, const Color& color);
	void DrawOBB(const OBB& box, const Color& color);
	void DrawFrustum(const Frustum& box, const Color& color);
	uint RenderSceneToTexture(std::vector<GameObject*>& scene_gos, std::string & out_path);

private:
	// --- Utilities ---
	void ClearRenderOrders();
	void UpdateGLCapabilities() const;
	uint CreateBufferFromData(uint Targetbuffer, uint size, void* data) const;
	void CreateFramebuffer();
	void CreateDefaultShaders();
	void CreateGrid(float target_distance);

private:

	// --- Draw ---
	void DrawRenderMeshes();
	void DrawRenderMesh(std::vector<RenderMesh> meshInstances);
	void HandleObjectOutlining();
	void DrawRenderLines();
	void DrawRenderBoxes();
	void DrawGrid();
	void DrawSkybox();

	// --- Draw Wireframe using given vertices ---
	template <typename Box>
	static void DrawWire(const Box& box, Color color, uint VAO)
	{
		float3 corners[8];
		box.GetCornerPoints(corners);
		DrawWireFromVertices(corners, color, VAO);
	};

	static void DrawWireFromVertices(const float3* corners, Color color, uint VAO);

public:
	// --- Default Shader ---
	ResourceShader* defaultShader = nullptr;
	ResourceShader* linepointShader = nullptr;
	ResourceShader* OutlineShader = nullptr;
	ResourceShader* ZDrawerShader = nullptr;
	ResourceShader* SkyboxShader = nullptr;
	ResourceShader* SkyboxReflectionShader = nullptr;
	ResourceShader* SkyboxRefractionShader = nullptr;

	std::string VertexShaderTemplate;
	std::string FragmentShaderTemplate;

	ComponentCamera* active_camera = nullptr;
	ComponentCamera* culling_camera = nullptr;
	ComponentCamera* screenshot_camera = nullptr;

	SDL_GLContext context;

	// --- Flags ---
	bool vsync = true;
	bool depth = true;
	bool cull_face = true;
	bool lighting = false;
	bool color_material = true;
	bool wireframe = false;
	bool zdrawer = false;
	bool renderfbo = true;
	bool display_boundingboxes = false;
	bool display_grid = true;

	uint rendertexture = 0;

private:
	std::map<uint, std::vector<RenderMesh>> render_meshes;

	std::vector<RenderBox<AABB>> render_aabbs;
	std::vector<RenderBox<OBB>> render_obbs;
	std::vector<RenderBox<Frustum>> render_frustums;
	std::vector<RenderLine> render_lines;

	uint fbo = 0;
	uint cubemapTexID = 0;
	uint skyboxVAO = 0;
	uint skyboxVBO = 0;
	uint depthbuffer = 0;
	uint PointLineVAO = 0;
	uint Grid_VAO = 0;
	uint Grid_VBO = 0;
	uint maxSimultaneousTextures = 0;
};
#endif