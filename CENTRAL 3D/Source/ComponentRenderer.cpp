#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ComponentRenderer::ComponentRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::Renderer)
{
}

ComponentRenderer::~ComponentRenderer()
{

}

void ComponentRenderer::Draw()
{
	ComponentMesh * mesh = (ComponentMesh*)this->GO->GetComponent(Component::ComponentType::Mesh);

	if (mesh)
	{
		DrawMesh(*mesh);
		DrawNormals(*mesh);
	}
}

void ComponentRenderer::DrawMesh(ComponentMesh& mesh) const
{

	// --- Draw Texture ---
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
	glEnable(GL_TEXTURE_2D); // enable gl capability

	// --- If the mesh has a material associated, get it ---
	ComponentMaterial* mat = (ComponentMaterial*)mesh.GetContainerGameObject()->GetComponent(Component::ComponentType::Material);

	if (mat)
	{
		glBindTexture(GL_TEXTURE_2D, mat->TextureID); // start using texture
		glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 
		glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
		glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format
	}

	// --- Draw mesh ---
	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	// ----        ----

	// --- Unbind buffers, Disable capabilities ---

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)
	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); // disable client-side capability
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should reset active texture
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)

}

void ComponentRenderer::DrawNormals(const ComponentMesh& mesh) const
{
	// --- Draw Mesh Normals ---

	if (mesh.Normals)
	{
		// --- Draw Vertex Normals ---

		glBegin(GL_LINES);
		glLineWidth(1.0f);

		glColor4f(0.0f, 0.5f, 0.5f, 1.0f);

		/*	for (uint j = 0; j < mesh->VerticesSize; ++j)
			{
				glVertex3f(mesh->Vertices[j].x, mesh->Vertices[j].y, mesh->Vertices[j].z);
				glVertex3f(mesh->Vertices[j].x + mesh->Normals[j].x*NORMAL_LENGTH, mesh->Vertices[j].y + mesh->Normals[j].y*NORMAL_LENGTH, mesh->Vertices[j].z + mesh->Normals[j].z*NORMAL_LENGTH);
			}
	*/
	// --- Draw Face Normals 

		Triangle face;

		for (uint j = 0; j < mesh.VerticesSize / 3; ++j)
		{
			face.a = mesh.Vertices[(j * 3)];
			face.b = mesh.Vertices[(j * 3) + 1];
			face.c = mesh.Vertices[(j * 3) + 2];

			float3 face_center = face.Centroid();
			//float3 face_normal = face.NormalCW();

			float3 face_normal = Cross(face.a - face.b, face.c - face.b);

			face_normal.Normalize();

			glVertex3f(face_center.x, face_center.y, face_center.z);
			glVertex3f(face_center.x + face_normal.x*NORMAL_LENGTH, face_center.y + face_normal.y*NORMAL_LENGTH, face_center.z + face_normal.z*NORMAL_LENGTH);
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnd();

	}

}