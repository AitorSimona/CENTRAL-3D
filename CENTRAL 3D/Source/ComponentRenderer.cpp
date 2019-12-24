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

#include "ResourceMesh.h"
#include "ResourceShader.h"

#include "mmgr/mmgr.h"

ComponentRenderer::ComponentRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::Renderer)
{
}

ComponentRenderer::~ComponentRenderer()
{

}

void ComponentRenderer::Draw() const
{
	ComponentMesh * mesh = this->GO->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
	ComponentCamera* camera = GO->GetComponent<ComponentCamera>(Component::ComponentType::Camera);
	ComponentMaterial* mat = GO->GetComponent<ComponentMaterial>(Component::ComponentType::Material);

	uint shader = App->renderer3D->defaultShader->ID;

	if(mat)
	shader = mat->resource_material->shader->ID;

	if (shader != App->renderer3D->defaultShader->ID)
		uint i = 3;

	glUseProgram(shader);

	GLint modelLoc = glGetUniformLocation(shader, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobalTransform().Transposed().ptr());

	if (mesh && mesh->resource_mesh && mesh->IsEnabled())
	{
		DrawMesh(*mesh->resource_mesh, mesh->GetContainerGameObject()->GetComponent<ComponentMaterial>(Component::ComponentType::Material));
		DrawNormals(*mesh->resource_mesh);
		DrawAxis();
	}

	glUseProgram(App->renderer3D->defaultShader->ID);

	// --- Draw Frustum ---
	if (camera)
		ModuleSceneManager::DrawWire(camera->frustum, White);

	if(App->scene_manager->display_boundingboxes)
	ModuleSceneManager::DrawWire(GO->GetAABB(), Green);

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

inline void ComponentRenderer::DrawNormals(const ResourceMesh& mesh) const
{
	// --- Draw Mesh Normals ---
	glLineWidth(2.0f);

	//glColor4f(0.0f, 0.5f, 0.5f, 1.0f);

	if (draw_vertexnormals && mesh.vertices->normal)
	{
		// --- Draw Vertex Normals ---

		for (uint j = 0; j < mesh.IndicesSize; ++j)
		{
			glVertex3f(mesh.vertices[mesh.Indices[j]].position[0], mesh.vertices[mesh.Indices[j]].position[1], mesh.vertices[mesh.Indices[j]].position[2]);
			glVertex3f(mesh.vertices[mesh.Indices[j]].position[0] + mesh.vertices[mesh.Indices[j]].normal[0]*NORMAL_LENGTH, mesh.vertices[mesh.Indices[j]].position[1] + mesh.vertices[mesh.Indices[j]].normal[1]*NORMAL_LENGTH, mesh.vertices[mesh.Indices[j]].position[2] + mesh.vertices[mesh.Indices[j]].normal[2]*NORMAL_LENGTH);
		}

	}
	
	// --- Draw Face Normals 

	if (draw_facenormals)
	{
		Triangle face;

		for (uint j = 0; j < mesh.IndicesSize / 3; ++j)
		{
			face.a = float3(mesh.vertices[mesh.Indices[j * 3]].position);
			face.b = float3(mesh.vertices[mesh.Indices[(j * 3) + 1]].position);
			face.c = float3(mesh.vertices[mesh.Indices[(j * 3) + 2]].position);

			float3 face_center = face.Centroid();

			float3 face_normal = Cross(face.b - face.a, face.c - face.b);

			face_normal.Normalize();

			glVertex3f(face_center.x, face_center.y, face_center.z);
			glVertex3f(face_center.x + face_normal.x*NORMAL_LENGTH, face_center.y + face_normal.y*NORMAL_LENGTH, face_center.z + face_normal.z*NORMAL_LENGTH);
		}

	}

	glLineWidth(1.0f);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

inline void ComponentRenderer::DrawAxis() const
{
	// --- Draw Axis ---
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	glEnd();

	glLineWidth(1.0f);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
}
