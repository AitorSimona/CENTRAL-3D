#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "OpenGL.h"

#include "Application.h"
#include "ModuleTextures.h"

#include "mmgr/mmgr.h"

ComponentRenderer::ComponentRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::Renderer)
{
}

ComponentRenderer::~ComponentRenderer()
{

}

void ComponentRenderer::Draw()
{
	ComponentMesh * mesh = this->GO->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

	if (mesh && mesh->IsEnabled())
	{
		DrawMesh(*mesh);
		DrawNormals(*mesh);
		DrawAxis();
	}
}

void ComponentRenderer::DrawMesh(ComponentMesh& mesh) const
{

	// --- Draw Texture ---
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
	glEnable(GL_TEXTURE_2D); // enable gl capability
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 

	// --- If the mesh has a material associated, get it ---
	ComponentMaterial* mat = mesh.GetContainerGameObject()->GetComponent<ComponentMaterial>(Component::ComponentType::Material);

	if (mat && mat->IsEnabled())
	{
		if(this->checkers)
		glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
		else
		glBindTexture(GL_TEXTURE_2D, mat->TextureID); // start using texture
		glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
		glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format
	}

	// --- Draw mesh ---
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, mesh.IndexDatatype, NULL); // render primitives from array data

	// ----        ----

	// --- Unbind buffers ---

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)

	// --- Disable capabilities ---
	glDisable(GL_TEXTURE_2D); // enable gl capability
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should reset active texture
	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); // disable client-side capability

}

void ComponentRenderer::DrawNormals(const ComponentMesh& mesh) const
{
	// --- Draw Mesh Normals ---
	glBegin(GL_LINES);
	glLineWidth(1.0f);

	glColor4f(0.0f, 0.5f, 0.5f, 1.0f);

	if (draw_vertexnormals && mesh.Normals)
	{
		// --- Draw Vertex Normals ---

		for (uint j = 0; j < mesh.IndicesSize; ++j)
		{
			glVertex3f(mesh.Vertices[mesh.Indices[j]].x, mesh.Vertices[mesh.Indices[j]].y, mesh.Vertices[mesh.Indices[j]].z);
			glVertex3f(mesh.Vertices[mesh.Indices[j]].x + mesh.Normals[mesh.Indices[j]].x*NORMAL_LENGTH, mesh.Vertices[mesh.Indices[j]].y + mesh.Normals[mesh.Indices[j]].y*NORMAL_LENGTH, mesh.Vertices[mesh.Indices[j]].z + mesh.Normals[mesh.Indices[j]].z*NORMAL_LENGTH);
		}

	}
	
	// --- Draw Face Normals 

	if (draw_facenormals)
	{
		Triangle face;

		for (uint j = 0; j < mesh.IndicesSize / 3; ++j)
		{
			face.a = mesh.Vertices[mesh.Indices[j * 3]];
			face.b = mesh.Vertices[mesh.Indices[(j * 3) + 1]];
			face.c = mesh.Vertices[mesh.Indices[(j * 3) + 2]];

			float3 face_center = face.Centroid();

			float3 face_normal = Cross(face.b - face.a, face.c - face.b);

			face_normal.Normalize();

			glVertex3f(face_center.x, face_center.y, face_center.z);
			glVertex3f(face_center.x + face_normal.x*NORMAL_LENGTH, face_center.y + face_normal.y*NORMAL_LENGTH, face_center.z + face_normal.z*NORMAL_LENGTH);
		}

	}

	glLineWidth(1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();

}

void ComponentRenderer::DrawAxis() const
{
	// --- Draw Axis ---
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

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
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
}
