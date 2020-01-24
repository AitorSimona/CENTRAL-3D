#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "Color.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTimeManager.h"
#include "ModuleWindow.h"

#include "ResourceMesh.h"
#include "ResourceShader.h"

#include "mmgr/mmgr.h"

ComponentRenderer::ComponentRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::Renderer)
{
}

ComponentRenderer::~ComponentRenderer()
{

}

void ComponentRenderer::Draw(bool outline) const
{
	ComponentMesh * mesh = this->GO->GetComponent<ComponentMesh>();
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	ComponentCamera* camera = GO->GetComponent<ComponentCamera>();
	ComponentMaterial* mat = GO->GetComponent<ComponentMaterial>();

	uint shader = App->renderer3D->defaultShader->ID;

	if(mat)
	shader = mat->resource_material->shader->ID;

	float4x4 model = transform->GetGlobalTransform();

	if (outline)
	{
		shader = App->renderer3D->OutlineShader->ID;
		// --- Draw selected, pass scaled-up matrix to shader ---
		float3 scale = float3(1.05f, 1.05f, 1.05f);
		
		model = float4x4::FromTRS(model.TranslatePart(), model.RotatePart(), scale);
	}

	mat->resource_material->UpdateUniforms();

	// --- Display Z buffer ---
	if (App->renderer3D->zdrawer)
	{
		shader = App->renderer3D->ZDrawerShader->ID;
	}

	glUseProgram(shader);

	// --- Set uniforms ---
	GLint modelLoc = glGetUniformLocation(shader, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.Transposed().ptr());
	
	GLint viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint timeLoc = glGetUniformLocation(shader, "time");
	glUniform1f(timeLoc, App->time->time);

	float farp = App->renderer3D->active_camera->GetFarPlane();
	float nearp = App->renderer3D->active_camera->GetNearPlane();
	// --- Give ZDrawer near and far camera frustum planes pos ---
	if (App->renderer3D->zdrawer)
	{
		int nearfarLoc = glGetUniformLocation(shader, "nearfar");
		glUniform2f(nearfarLoc, nearp, farp);
	}

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
	 float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(shader, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());


	if (mesh && mesh->resource_mesh && mesh->IsEnabled())
	{
		DrawMesh(*mesh->resource_mesh, mesh->GetContainerGameObject()->GetComponent<ComponentMaterial>());
		DrawNormals(*mesh->resource_mesh,*transform);
	}

	glUseProgram(App->renderer3D->defaultShader->ID);

	// --- Draw Frustum ---
	if (camera)
		ModuleSceneManager::DrawWire(camera->frustum, White, App->scene_manager->GetPointLineVAO());

	if(App->scene_manager->display_boundingboxes)
	ModuleSceneManager::DrawWire(GO->GetAABB(), Green, App->scene_manager->GetPointLineVAO());
}

inline void ComponentRenderer::DrawMesh(ResourceMesh& mesh, ComponentMaterial* mat) const
{
	glBindVertexArray(mesh.VAO);

	if (mat && mat->IsEnabled())
	{
		if (this->checkers)
			glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
		else
			glBindTexture(GL_TEXTURE_2D, mat->resource_material->resource_diffuse->buffer_id); // start using texture
	}


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
}

void ComponentRenderer::DrawNormals(const ResourceMesh& mesh, const ComponentTransform& transform) const
{
	// --- Draw Mesh Normals ---

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->linepointShader->ID);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.GetGlobalTransform().Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	int vertexColorLocation = glGetAttribLocation(App->renderer3D->linepointShader->ID, "color");
	glVertexAttrib3f(vertexColorLocation, 1.0, 1.0, 0);

	if (draw_vertexnormals && mesh.vertices->normal)
	{
		// --- Draw Vertex Normals ---

		float3* vertices = new float3[mesh.IndicesSize * 2];

		for (uint i = 0; i < mesh.IndicesSize; ++i)
		{
			// --- Normals ---
			vertices[i * 2] = float3(mesh.vertices[mesh.Indices[i]].position[0], mesh.vertices[mesh.Indices[i]].position[1], mesh.vertices[mesh.Indices[i]].position[2]);
			vertices[(i * 2) + 1] = float3(mesh.vertices[mesh.Indices[i]].position[0] + mesh.vertices[mesh.Indices[i]].normal[0] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[1] + mesh.vertices[mesh.Indices[i]].normal[1] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[2] + mesh.vertices[mesh.Indices[i]].normal[2] * NORMAL_LENGTH);
		}

		// --- Create VAO, VBO ---
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(App->scene_manager->GetPointLineVAO());

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh.IndicesSize*2, vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		// --- Draw lines ---
		glLineWidth(3.0f);
		glBindVertexArray(App->scene_manager->GetPointLineVAO());
		glDrawArrays(GL_LINES, 0, mesh.IndicesSize * 2);
		glBindVertexArray(0);
		glLineWidth(1.0f);


		// --- Delete VBO and vertices ---
		glDeleteBuffers(1, &VBO);
		delete[] vertices;
	}
	
	// --- Draw Face Normals 

	if (draw_facenormals)
	{
		Triangle face;
		float3* vertices = new float3[mesh.IndicesSize/3*2];
		
		// --- Compute face normals ---
		for (uint j = 0; j < mesh.IndicesSize / 3; ++j)
		{
			face.a = float3(mesh.vertices[mesh.Indices[j * 3]].position);
			face.b = float3(mesh.vertices[mesh.Indices[(j * 3) + 1]].position);
			face.c = float3(mesh.vertices[mesh.Indices[(j * 3) + 2]].position);

			float3 face_center = face.Centroid();

			float3 face_normal = Cross(face.b - face.a, face.c - face.b);

			face_normal.Normalize();

			vertices[j*2] = float3(face_center.x, face_center.y, face_center.z);
			vertices[(j*2) + 1] = float3(face_center.x + face_normal.x*NORMAL_LENGTH, face_center.y + face_normal.y*NORMAL_LENGTH, face_center.z + face_normal.z*NORMAL_LENGTH);
		}

		// --- Create VAO, VBO ---
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(App->scene_manager->GetPointLineVAO());

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3 * mesh.IndicesSize / 3 * 2, vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// --- Draw lines ---

		glLineWidth(3.0f);
		glBindVertexArray(App->scene_manager->GetPointLineVAO());
		glDrawArrays(GL_LINES, 0, mesh.IndicesSize / 3 * 2);
		glBindVertexArray(0);
		glLineWidth(1.0f);

		// --- Delete VBO and vertices ---
		glDeleteBuffers(1, &VBO);
		delete[] vertices;
	}

	glUseProgram(App->renderer3D->defaultShader->ID);
}

