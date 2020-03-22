#ifndef __BE_MODULERENDERER_H__
#define __BE_MODULERENDERER_H__

#include "Module.h"
#include "Light.h"
#include "JSONLoader.h"

#define MAX_LIGHTS 8

BE_BEGIN_NAMESPACE
class ComponentCamera;
class ResourceShader;
class ResourceMesh;
class ResourceMaterial;
class math::float4x4;

typedef int RenderMeshFlags;

enum RenderMeshFlags_
{
	None		= 0,
	outline		= 1 << 0,
	selected	= 1 << 1,
	checkers	= 1 << 2,
	wire		= 1 << 3,
	texture		= 1 << 4
};

struct RenderMesh
{
	RenderMesh(float4x4 transform, const ResourceMesh* mesh, const ResourceMaterial* mat, const RenderMeshFlags flags = 0) : transform(transform), resource_mesh(mesh), mat(mat), flags(flags) {}

	float4x4 transform;
	const ResourceMesh* resource_mesh = nullptr;
	const ResourceMaterial* mat = nullptr;
	//const Color color = White; // force a color draw, useful if no texture is given

	// temporal!
	const ResourceMesh* deformable_mesh = nullptr;


	// --- Add rendering options here ---
	RenderMeshFlags flags = None;
};

template <typename Box>
struct RenderBox
{
	RenderBox(const Box* box, const Color& color) : box(box), color(color) {}

	const Box* box;
	Color color;
};

struct RenderLine
{
	RenderLine(float4x4 transform, const float3& a, const float3& b, const Color& color) : transform(transform), a(a), b(b), color(color) {}

	float4x4 transform;
	float3 a;
	float3 b;
	Color color;
};

class BROKEN_API ModuleRenderer3D : public Module {
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(json& file) override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	// --- Utilities ---
	void UpdateGLCapabilities() const;
	void OnResize(int width, int height);
	uint CreateBufferFromData(uint Targetbuffer, uint size, void* data) const;
	void CreateFramebuffer();

	// --- Setters ---
	bool SetVSync(bool _vsync);
	void SetActiveCamera(ComponentCamera* camera);
	void SetCullingCamera(ComponentCamera* camera);

	// --- Getters ---
	bool GetVSync() const;

	// --- Render orders --- // Deformable mesh is Temporal!
	void DrawMesh(const float4x4 transform, const ResourceMesh* mesh, const ResourceMaterial* mat, const ResourceMesh* deformable_mesh = nullptr, const RenderMeshFlags flags = 0);
	void DrawLine(const float4x4 transform, const float3 a, const float3 b, const Color& color);
	void DrawAABB(const AABB& box, const Color& color);
	void DrawFrustum(const Frustum& box, const Color& color);


private:
	void HandleObjectOutlining();
	void CreateDefaultShaders();
	void ClearRenderOrders();

	// --- Draw ---
	void DrawRenderMeshes();
	void DrawRenderMesh(std::vector<RenderMesh> meshInstances);

	void DrawRenderLines();
	void DrawRenderBoxes();

public:
	// --- Default Shader ---
	ResourceShader* defaultShader = nullptr;
	ResourceShader* linepointShader = nullptr;
	ResourceShader* OutlineShader = nullptr;
	ResourceShader* ZDrawerShader = nullptr;
	ResourceShader* textShader = nullptr;

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
	bool renderfbo = true;

private:
	std::map<uint, std::vector<RenderMesh>> render_meshes;

	std::vector<RenderBox<AABB>> render_aabbs;
	std::vector<RenderBox<Frustum>> render_frustums;
	std::vector<RenderLine> render_lines;
};
BE_END_NAMESPACE
#endif