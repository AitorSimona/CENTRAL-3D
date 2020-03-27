#include "ModuleDetour.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourDebugDraw.h"
#include "DebugDraw.h"
#include "ResourceNavMesh.h"
#include "ImporterNavMesh.h"
#include "ModuleResourceManager.h"
#include "Application.h"
#include "Math.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

using namespace Broken;

//Debug drawing functions

class GLCheckerTexture {
	unsigned int m_texId;
public:
	GLCheckerTexture() : m_texId(0) {
	}

	~GLCheckerTexture() {
		if (m_texId != 0)
			glDeleteTextures(1, &m_texId);
	}
	void bind() {
		if (m_texId == 0) {
			// Create checker pattern.
			const unsigned int col0 = duRGBA(215, 215, 215, 255);
			const unsigned int col1 = duRGBA(255, 255, 255, 255);
			static const int TSIZE = 64;
			unsigned int data[TSIZE * TSIZE];

			glGenTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_2D, m_texId);

			int level = 0;
			int size = TSIZE;
			while (size > 0) {
				for (int y = 0; y < size; ++y)
					for (int x = 0; x < size; ++x)
						data[x + y * size] = (x == 0 || y == 0) ? col0 : col1;
				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				size /= 2;
				level++;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}
	}
};
GLCheckerTexture g_tex;

BE_BEGIN_NAMESPACE
class DebugDrawGL : public duDebugDraw {
public:
	void depthMask(bool state) {
		glDepthMask(state ? GL_TRUE : GL_FALSE);
	}
	void texture(bool state) {
		if (state) {
			glEnable(GL_TEXTURE_2D);
			g_tex.bind();
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}
	}

	void begin(duDebugDrawPrimitives prim, float size) {
		//Set polygon draw mode and appropiated matrices for OGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/*glPushMatrix();
		glMultMatrixf(float4x4::identity.Transposed().ptr());
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(App->camera->camera->GetOpenGLProjectionMatrix().Transposed().ptr());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(App->camera->camera->GetOpenGLViewMatrix().Transposed().ptr());*/


		switch (prim) {

		case DU_DRAW_POINTS:
			glPointSize(size);
			glBegin(GL_POINTS);
			break;
		case DU_DRAW_LINES:
			glLineWidth(size);
			glBegin(GL_LINES);
			break;
		case DU_DRAW_TRIS:
			glBegin(GL_TRIANGLES);
			break;
		case DU_DRAW_QUADS:
			glBegin(GL_QUADS);
			break;
		};
	}
	void vertex(const float* pos, unsigned int color) {
		glColor4ubv((GLubyte*)&color);
		glVertex3fv(pos);
	}	
	void vertex(const float x, const float y, const float z, unsigned int color) {
		glColor4ubv((GLubyte*)&color);
		glVertex3f(x, y, z);
	}

	void vertex(const float* pos, unsigned int color, const float* uv) {
		glColor4ubv((GLubyte*)&color);
		glTexCoord2fv(uv);
		glVertex3fv(pos);
	}

	void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) {
		glColor4ubv((GLubyte*)&color);
		glTexCoord2f(u, v);
		glVertex3f(x, y, z);
	}

	void end() {
		glEnd();
		glLineWidth(1.0f);
		glPointSize(1.0f);
	}
};


BE_END_NAMESPACE
////////////////////////////////////////////////////////////////////////////////////////////////////



ModuleDetour::ModuleDetour(bool start_enabled) : Module(start_enabled){
	name = "Detour";
}

ModuleDetour::~ModuleDetour() {
}

bool ModuleDetour::Init(json& config) {

	for (int i = 0; i < BE_DETOUR_TOTAL_AREAS; ++i) {
		sprintf_s(areaNames[i], "");
		areaCosts[i] = 1;
	}

	//Inititalize names
	sprintf_s(areaNames[0], "Walkable");
	sprintf_s(areaNames[1], "Not Walkable");
	sprintf_s(areaNames[2], "Jump");

	//Change value of Jump to 2 by default
	areaCosts[2] = 2;

	agentRadius = 0.5f;
	agentHeight = 2.0f;
	maxSlope = 45.0f;
	stepHeight = 0.4f;

	m_navQuery = dtAllocNavMeshQuery();
	m_dd = new DebugDrawGL();

	return true;
}

bool ModuleDetour::createNavMesh(dtNavMeshCreateParams* params) {

	ENGINE_AND_SYSTEM_CONSOLE_LOG("Starting NavMesh build");
	unsigned char* navData = 0;
	int navDataSize = 0;

	dtNavMesh* m_navMesh = nullptr;
	if (navMeshResource != nullptr)
		m_navMesh = navMeshResource->navMesh;

	dtFreeNavMesh(m_navMesh);
	m_navMesh = nullptr;

	if (!dtCreateNavMeshData(params, &navData, &navDataSize)) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not build Detour navmesh.");
		return false;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh) {
		dtFree(navData);
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not create Detour navmesh");
		return false;
	}

	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status)) {
		dtFree(navData);
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not init Detour navmesh");
		return false;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status)) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not init Detour navmesh query");
		return false;
	}

	if (navMeshResource == nullptr) {
		std::string resourceName = App->scene_manager->currentScene->GetName();
		resourceName.erase(resourceName.find('.'));
		resourceName = NAVMESH_FOLDER + resourceName + ".navmesh";
		navMeshResource = (ResourceNavMesh*)App->resources->CreateResource(Resource::ResourceType::NAVMESH, resourceName.c_str());

		ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
		ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Broken::Resource::ResourceType::META, navMeshResource->GetResourceFile(), navMeshResource->GetUID());

		if (meta)
			IMeta->Save(meta);

		App->resources->AddResourceToFolder(navMeshResource);

	}
	navMeshResource->navMesh = m_navMesh;


	ImporterNavMesh* INavMesh = App->resources->GetImporter<ImporterNavMesh>();
	INavMesh->Save(navMeshResource);

	//If we are in the editor we need to create the draw meshes
	if (!App->isGame)
		createRenderMeshes();

	//We save the scene so that it stores the NavMesh
	App->scene_manager->SaveScene(App->scene_manager->currentScene);

	return true;
}

void ModuleDetour::loadNavMeshFile(uint UID) {
	if (navMeshResource != nullptr)
		navMeshResource->Release();

	navMeshResource = (ResourceNavMesh*)App->resources->GetResource(UID, true);

	if (navMeshResource != nullptr) {
		dtStatus status = m_navQuery->init(navMeshResource->navMesh, 2048);
		if (dtStatusFailed(status)) {
			ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not init Detour navmesh query");
		}
	}

	if (!App->isGame) {
		for (int i = 0; i < renderMeshes.size(); ++i)
			delete renderMeshes[i];
		renderMeshes.clear();
		createRenderMeshes();
	}
}

bool ModuleDetour::CleanUp() {
	if (navMeshResource != nullptr)
		navMeshResource->Release();
	navMeshResource = nullptr;

	for (int i = 0; i < renderMeshes.size(); ++i)
		delete renderMeshes[i];
	renderMeshes.clear();

	dtFreeNavMeshQuery(m_navQuery);
	m_navQuery = nullptr;

	if (m_dd != nullptr)
		delete m_dd;

	m_dd = nullptr;

	return true;
}



void ModuleDetour::Draw() const {
	if (debugDraw && navMeshResource != nullptr && navMeshResource->navMesh != nullptr) {
		for (int i = 0; i < renderMeshes.size(); ++i)
			App->renderer3D->DrawMesh(float4x4::identity, renderMeshes[i]->rmesh, mat, nullptr, 0, renderMeshes[i]->color);
	}
}

void ModuleDetour::setDebugDraw(bool state) {
	debugDraw = state;
}

void ModuleDetour::deleteNavMesh() {
	if (navMeshResource != nullptr) {
		navMeshResource->OnDelete();
		delete navMeshResource;
		navMeshResource = nullptr;
	}
	for (int i = 0; i < renderMeshes.size(); ++i)
		delete renderMeshes[i];
	renderMeshes.clear();

	//We save the scene so that it stores that we no longer have a navmesh
	App->scene_manager->SaveScene(App->scene_manager->currentScene);
}

void ModuleDetour::clearNavMesh() {
	if (navMeshResource != nullptr)
		navMeshResource->FreeMemory();

	navMeshResource = nullptr;
	for (int i = 0; i < renderMeshes.size(); ++i)
		delete renderMeshes[i];
	renderMeshes.clear();
}

void ModuleDetour::setDefaultValues() {
	for (int i = 3; i < BE_DETOUR_TOTAL_AREAS; ++i) {
		sprintf_s(areaNames[i], "");
		areaCosts[i] = 1;
	}

	//Inititalize names
	sprintf_s(areaNames[0], "Walkable");
	sprintf_s(areaNames[1], "Not Walkable");
	sprintf_s(areaNames[2], "Jump");

	//Change value of Jump to 2 by default
	areaCosts[2] = 2;

	setDefaultBakeValues();
}

void ModuleDetour::setDefaultBakeValues() {
	agentRadius = 0.5f;
	agentHeight = 2.0f;
	maxSlope = 45.0f;
	stepHeight = 0.4f;
	voxelSize = 0.15f;
	voxelHeight = 0.2f;

	regionMinSize = 8;
	regionMergeSize = 20;
	edgeMaxLen = 12.0f;
	edgeMaxError = 1.3f;
	vertsPerPoly = 6.0f;
	detailSampleDist = 6.0f;
	detailSampleMaxError = 1.0f;
}

const ResourceNavMesh* ModuleDetour::getNavMeshResource() const {
	return navMeshResource;
}

void ModuleDetour::createRenderMeshes() {
	if (navMeshResource != nullptr && navMeshResource->navMesh != nullptr) {
		const dtNavMesh* mesh = navMeshResource->navMesh;
		for (int ti = 0; ti < mesh->getMaxTiles(); ++ti) {
			const dtMeshTile* tile = mesh->getTile(ti);
			if (!tile->header) continue;
			processTile(tile);
		}
		if (mat == nullptr)
			mat = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID(), true);
	}
}

void ModuleDetour::processTile(const dtMeshTile* tile) {
	for (int i = 0; i < tile->header->polyCount; ++i) {
		const dtPoly* poly = &tile->polys[i];
		if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* poly_d = &tile->detailMeshes[i];
		navigationPoly* navpol = new navigationPoly();
		navpol->color = areaToColor(poly->getArea());
		navpol->rmesh->VerticesSize = poly->vertCount + poly_d->vertCount;
		navpol->rmesh->vertices = new Vertex[navpol->rmesh->VerticesSize];
		navpol->rmesh->IndicesSize = poly_d->triCount * 3;
		navpol->rmesh->Indices = new uint[navpol->rmesh->IndicesSize];

		// Index pointer to copy the indices
		uint* index_indices = navpol->rmesh->Indices;
		for (int j = 0; j < poly_d->triCount; ++j) {
			const unsigned char* t = &tile->detailTris[(poly_d->triBase + j) * 4];
			memcpy(index_indices, t, sizeof(uint) * 3);
			index_indices += 3;
		}

		// We copy the vertices
		for (int j = 0; j < navpol->rmesh->VerticesSize; ++j) {
			float* vert;
			if (j < poly->vertCount)
				vert = &tile->verts[poly->verts[j * 3]];
			else
				vert = &tile->detailVerts[(poly_d->vertBase + j - poly->vertCount) * 3];

			memcpy(navpol->rmesh->vertices[j].position, vert, sizeof(float) * 3);
		}

		//To create EBO and VBO
		navpol->rmesh->LoadInMemory();
		renderMeshes.push_back(navpol);
	}

}

inline int bit(int a, int b) {
	return (a & (1 << b)) >> b;
}

Color ModuleDetour::areaToColor(uint area) const {
	int	r = (bit(area, 1) + bit(area, 3) * 2 + 1);
	int	g = (bit(area, 2) + bit(area, 4) * 2 + 1);
	int	b = (bit(area, 0) + bit(area, 5) * 2 + 1);

	return Color(r, g, b, 255);
}

navigationPoly::navigationPoly() {
	rmesh = new ResourceMesh(App->GetRandom().Int(), "NULLFILE");
}

navigationPoly::~navigationPoly() {
	if (rmesh) {
		rmesh->FreeMemory();
		delete rmesh;
	}
}
