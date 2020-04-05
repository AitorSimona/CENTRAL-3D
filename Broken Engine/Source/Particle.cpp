#include "Particle.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"

#include "ResourceTexture.h"

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
	/*
	float4x4 transform=float4x4::identity;
	transform.SetIdentity();
	transform = float4x4::FromTRS(position,Quat::identity,float3::one);

	RenderMeshFlags flags = None;
	App->renderer3D->DrawMesh(transform, App->scene_manager->GetSphereMesh(), (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID(), false), nullptr, flags, Color(color.x, color.y, color.z,1.0f));
	*/

	//glUseProgram(App->renderer3D->defaultShader->ID);

	//float4x4 transform=float4x4::identity;
	//transform.SetIdentity();
	//transform = float4x4::FromTRS(position,Quat::identity,float3::one);

	//// --- Set uniforms ---
	//GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	//GLint viewLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "view");
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	//float farp = App->renderer3D->active_camera->GetFarPlane();
	//float nearp = App->renderer3D->active_camera->GetNearPlane();

	//// --- Give ZDrawer near and far camera frustum planes pos ---
	//if (App->renderer3D->zdrawer)
	//{
	//	int nearfarLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "nearfar");
	//	glUniform2f(nearfarLoc, nearp, farp);
	//}

	//// right handed projection matrix
	//float f = diameter / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	//float4x4 proj_RH(
	//	f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
	//	0.0f, f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, -1.0f,
	//	0.0f, 0.0f, nearp, 0.0f);

	//GLint projectLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "projection");
	//glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	//
	//const ResourceMesh* particle_sphere = App->scene_manager->GetSphereMesh();
	////App->scene_manager->CreateSphere(1, 4,4,particle_sphere);

	//int vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
	//glUniform3f(vertexColorLocation, color.x, color.y, color.z);

	//int TextureSupportLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
	//glUniform1i(TextureSupportLocation, -1);

	//// --- Draw particles (we use a sphere) ---
	//if (particle_sphere->vertices && particle_sphere->Indices)
	//{
	//	glBindVertexArray(particle_sphere->VAO);

	//	glBindTexture(GL_TEXTURE_2D, App->textures->GetDefaultTextureID());


	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_sphere->EBO);
	//	glDrawElements(GL_TRIANGLES, particle_sphere->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	//	glBindVertexArray(0);
	//	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
	//}

	//// --- Set uniforms back to defaults ---
	//glUniform3f(vertexColorLocation, 255, 255, 255);
	//glUniform1i(TextureSupportLocation, 0);

	//glUseProgram(App->renderer3D->defaultShader->ID);


	// --- Update transform and rotation to face camera ---
	float3 frustum_pos = float3(0, 0, 0);
	float3 center = float3(position.x, position.y, position.z);

	// --- Frame image with camera ---
	float4x4 transform = transform.FromTRS(float3(position.x, position.y, position.z),
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
		float3(scale.x, scale.y, 1));

	float3 Movement = App->renderer3D->active_camera->frustum.Front();
	//float3 camera_pos = frustum_pos;
	/*
	if (Movement.IsFinite())
		App->renderer3D->active_camera->frustum.SetPos(center - Movement);*/

	// --- Set Uniforms ---
	uint shaderID = App->renderer3D->defaultShader->ID;
	glUseProgram(shaderID);

	GLint modelLoc = glGetUniformLocation(shaderID, "u_Model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(shaderID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		position.x * 0.01f, position.y * 0.01f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(shaderID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	//Texturing & Color
	GLint vertexColorLocation = glGetUniformLocation(shaderID, "u_Color");
	glUniform3f(vertexColorLocation, color.x, color.y, color.z);

	int TextureLocation = glGetUniformLocation(shaderID, "u_UseTextures");
	glUniform1i(TextureLocation, (int)true);
	//ourTexture
	glUniform1i(glGetUniformLocation(shaderID, "u_AlbedoTexture"), 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTexID());

	// --- Draw plane with given texture ---
	glBindVertexArray(App->scene_manager->plane->VAO);

	//glBindTexture(GL_TEXTURE_2D, texture->GetTexID());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->plane->EBO);
	glDrawElements(GL_TRIANGLES, App->scene_manager->plane->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glUniform1i(TextureLocation, 0); //reset texture location
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
	// --- Set camera back to original position ---
	//App->renderer3D->active_camera->frustum.SetPos(camera_pos);


}
//
//// --- Update transform and rotation to face camera ---
//float3 frustum_pos = float3(0, 0, 0);
//float3 center = float3(frustum_pos.x, frustum_pos.y, 0);
//
//// --- Frame image with camera ---
//float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, 0),
//	App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
//	float3(size2D, 1));
//
//float3 Movement = App->renderer3D->active_camera->frustum.Front();
//float3 camera_pos = frustum_pos;
///*
//if (Movement.IsFinite())
//	App->renderer3D->active_camera->frustum.SetPos(center - Movement);*/
//
//	// --- Set Uniforms ---
//glUseProgram(App->renderer3D->defaultShader->ID);
//
//int TextureLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
//glUniform1i(TextureLocation, -1);
//GLint vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
//glUniform3f(vertexColorLocation, 1.0f, 1.0f, 1.0f);
//
//GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());
//
//GLint viewLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "view");
//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());
//
//float nearp = App->renderer3D->active_camera->GetNearPlane();
//
//// right handed projection matrix
//float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
//float4x4 proj_RH(
//	f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
//	0.0f, f, 0.0f, 0.0f,
//	0.0f, 0.0f, 0.0f, -1.0f,
//	position2D.x * 0.01f, position2D.y * 0.01f, nearp, 0.0f);
//
//GLint projectLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "projection");
//glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());
//
//glUniform1i(TextureLocation, 0); //reset texture location
//
//// --- Draw plane with given texture ---
//glBindVertexArray(App->scene_manager->plane->VAO);
//
//glBindTexture(GL_TEXTURE_2D, texture->GetTexID());
//
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->plane->EBO);
//glDrawElements(GL_TRIANGLES, App->scene_manager->plane->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data
//
//glBindVertexArray(0);
//glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
//
//
//// --- Set camera back to original position ---
////App->renderer3D->active_camera->frustum.SetPos(camera_pos);
