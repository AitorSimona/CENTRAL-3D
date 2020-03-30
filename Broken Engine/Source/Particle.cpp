#include "Particle.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"

using namespace Broken;

Particle::Particle()
{
	position = { 0.0f,0.0f,0.0f };
	lifeTime = 1000;
	diameter = 1;
}

Particle::~Particle()
{}

void Particle::Draw()
{
	float4x4 transform=float4x4::identity;
	transform.SetIdentity();
	transform = float4x4::FromTRS(position,Quat::identity,float3::one);

	RenderMeshFlags flags = None;
	App->renderer3D->DrawMesh(transform, App->scene_manager->GetSphereMesh(), (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID(), false), nullptr, flags, Color(color.x, color.y, color.z,1.0f));
}
