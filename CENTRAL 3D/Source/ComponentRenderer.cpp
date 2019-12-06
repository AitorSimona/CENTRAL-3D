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

	// --- Send transform to OpenGL and use it to draw ---
	//glPushMatrix();
	//glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());


	//glUseProgram(App->renderer3D->shaderProgram);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->shaderProgram, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobalTransform().Transposed().ptr());

	//float3 view_pos = view.RotatePart().Transposed().Transform(-view.TranslatePart());



	if (mesh && mesh->resource_mesh && mesh->IsEnabled())
	{
		DrawMesh(*mesh->resource_mesh, mesh->GetContainerGameObject()->GetComponent<ComponentMaterial>(Component::ComponentType::Material));
		DrawNormals(*mesh->resource_mesh);
		DrawAxis();
	}

	// --- Pop transform so OpenGL does not use it for other operations ---
	//glPopMatrix();

	// --- Draw Frustum ---
	if (camera)
		ModuleSceneManager::DrawWire(camera->frustum, White);

	if(App->scene_manager->display_boundingboxes)
	ModuleSceneManager::DrawWire(GO->GetAABB(), Green);

}

inline void ComponentRenderer::DrawMesh(ResourceMesh& mesh, ComponentMaterial* mat) const
{
	// --- Draw Texture ---
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
	//glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
	//glEnable(GL_TEXTURE_2D); // enable gl capability
	//glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 

	// --- If the mesh has a material associated, use it ---

	//// --- Draw mesh ---
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer (vertices)
	//glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer (indices)


	//int vertexColorLocation = glGetAttribLocation(App->renderer3D->shaderProgram, "color");
	//glVertexAttrib3f(vertexColorLocation, 0.0f, 1.0f, 0.0f);

	//glUseProgram(App->renderer3D->shaderProgram);
	glBindVertexArray(mesh.VAO);

	if (mat && mat->IsEnabled())
	{
		if (this->checkers)
			glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
		else
			glBindTexture(GL_TEXTURE_2D, mat->resource_material->resource_diffuse->buffer_id); // start using texture
			//glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
			//glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format
	}


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)

	//glUseProgram(0);

	//glUseProgram(0);
	//// ----        ----

	//// --- Unbind buffers ---

	//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

	//// --- Disable capabilities ---
	//glDisable(GL_TEXTURE_2D); // enable gl capability
	//glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should reset active texture
	//glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY); // disable client-side capability

}

inline void ComponentRenderer::DrawNormals(const ResourceMesh& mesh) const
{
	// --- Draw Mesh Normals ---
	glBegin(GL_LINES);
	glLineWidth(1.0f);

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
	glEnd();

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
