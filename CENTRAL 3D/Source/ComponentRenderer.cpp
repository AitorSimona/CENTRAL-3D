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
		//DrawNormals(*mesh);
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
	ComponentMaterial* mat = (ComponentMaterial*)mesh.GetContainerGameObject()->GetComponent(Component::ComponentType::Material);

	if (mat)
	{
		glBindTexture(GL_TEXTURE_2D, mat->TextureID); // start using texture
		glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
		glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format
	}

	// --- Draw mesh ---
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

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

	// Test code to draw axis on object, to be adapted
	//if (axis == true)
	//{
	//	// Draw Axis Grid
	//	glLineWidth(2.0f);

	//	glBegin(GL_LINES);


	//	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	//	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	//	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	//	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	//	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	//	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	//	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	//	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	//	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	//	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	//	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	//	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	//	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	//	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	//	glEnd();

	//	glLineWidth(1.0f);
	//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//}

}