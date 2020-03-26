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
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"

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

	//We save the scene so that it stores the NavMesh
	App->scene_manager->SaveScene(App->scene_manager->currentScene);

	return true;
}

void ModuleDetour::loadNavMeshFile(uint UID) {
	if (navMeshResource != nullptr)
		navMeshResource->Release();

	navMeshResource = (ResourceNavMesh*) App->resources->GetResource(UID, true);
}

bool ModuleDetour::CleanUp() {
	navMeshResource = nullptr;

	dtFreeNavMeshQuery(m_navQuery);
	m_navQuery = nullptr;

	if (m_dd != nullptr)
		delete m_dd;

	m_dd = nullptr;

	return true;
}



void ModuleDetour::Draw() const {
	if (debugDraw && navMeshResource != nullptr && navMeshResource->navMesh != nullptr) {
		unsigned char flags = 0;
		duDebugDrawNavMesh((duDebugDraw*)m_dd, *(navMeshResource->navMesh), flags);
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

	//We save the scene so that it stores that we no longer have a navmesh
	App->scene_manager->SaveScene(App->scene_manager->currentScene);
}

void ModuleDetour::clearNavMesh() {
	if (navMeshResource != nullptr)
		navMeshResource->FreeMemory();

	navMeshResource = nullptr;
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
