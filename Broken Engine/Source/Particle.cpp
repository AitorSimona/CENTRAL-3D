#include "Particle.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"
#include "ModuleTextures.h"
#include "OpenGL.h"

#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"

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
	glUseProgram(App->renderer3D->defaultShader->ID);

	float4x4 transform=float4x4::identity;
	transform.SetIdentity();
	transform = float4x4::FromTRS(position,Quat::identity,float3::one);

	// --- Set uniforms ---
	GLint modelLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float farp = App->renderer3D->active_camera->GetFarPlane();
	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// --- Give ZDrawer near and far camera frustum planes pos ---
	if (App->renderer3D->zdrawer)
	{
		int nearfarLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "nearfar");
		glUniform2f(nearfarLoc, nearp, farp);
	}

	// right handed projection matrix
	float f = diameter / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->defaultShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	
	const ResourceMesh* particle_sphere = App->scene_manager->GetSphereMesh();
	//App->scene_manager->CreateSphere(1, 4,4,particle_sphere);

	int vertexColorLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Color");
	glUniform3f(vertexColorLocation, color.x, color.y, color.z);

	int TextureSupportLocation = glGetUniformLocation(App->renderer3D->defaultShader->ID, "Texture");
	glUniform1i(TextureSupportLocation, -1);

	// --- Draw particles (we use a sphere) ---
	if (particle_sphere->vertices && particle_sphere->Indices)
	{
		glBindVertexArray(particle_sphere->VAO);

		glBindTexture(GL_TEXTURE_2D, App->textures->GetDefaultTextureID());


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_sphere->EBO);
		glDrawElements(GL_TRIANGLES, particle_sphere->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
	}

	// --- Set uniforms back to defaults ---
	glUniform3f(vertexColorLocation, 255, 255, 255);
	glUniform1i(TextureSupportLocation, 0);

	glUseProgram(App->renderer3D->defaultShader->ID);
}
