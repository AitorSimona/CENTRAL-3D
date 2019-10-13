#include "Globals.h"
#include "OpenGL.h"
#include "Primitive.h"
#include "par/par_shapes.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"

#include "mmgr/mmgr.h"

// ------------------------------------------------------------
Primitive::Primitive() : transform(math::float4x4::identity), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Cube)
{}

Primitive::~Primitive()
{
	glDeleteBuffers(1, (GLuint*)&mesh.VerticesID);
	glDeleteBuffers(1, (GLuint*)&mesh.IndicesID);

	RELEASE_ARRAY(mesh.Vertices);
	RELEASE_ARRAY(mesh.Indices);
}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf((transform).ptr());

	if (axis == true)
	{
		// Draw Axis Grid
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

	// --- Activate wireframe mode ---
	if (wire || App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	InnerRender();

	// --- DeActivate wireframe mode ---
	if (wire || App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	

	glPopMatrix();
}

void Primitive::InnerRender() const
{

	glColor3f(color.r, color.g, color.b);

	// -----

	// --- Draw Texture ---
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
	glEnable(GL_TEXTURE_2D); // enable gl capability
	glBindTexture(GL_TEXTURE_2D, mesh.TextureID); // start using texture
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 
	glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
	glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format

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


	// -----

	glColor3f(1.0f, 1.0f, 1.0f);
}


// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.ptr()[12] = x;
	transform.ptr()[13] = y;
	transform.ptr()[14] = z;
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const float3 &u)
{
	// --- Angle in RADIANS ---
	angle = angle / 180.0f * (float)M_PI;

	// --- Normalize vector ---
	float3 v = u.Normalized();

	float c = 1.0f - cosf(angle), s = sinf(angle);

	transform.ptr()[0] = 1.0f + c * (v.x * v.x - 1.0f);
	transform.ptr()[1] = c * v.x * v.y + v.z * s;
	transform.ptr()[2] = c * v.x * v.z - v.y * s;
	transform.ptr()[4] = c * v.x * v.y - v.z * s;
	transform.ptr()[5] = 1.0f + c * (v.y * v.y - 1.0f);
	transform.ptr()[6] = c * v.y * v.z + v.x * s;
	transform.ptr()[8] = c * v.x * v.z + v.y * s;
	transform.ptr()[9] = c * v.y * v.z - v.x * s;
	transform.ptr()[10] = 1.0f + c * (v.z * v.z - 1.0f);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.ptr()[0] = x;
	transform.ptr()[5] = y;
	transform.ptr()[10] = z;
}

// CUBE ============================================


PrimitiveCube::PrimitiveCube(float sizeX, float sizeY, float sizeZ, bool checkers)
{
	type = type = PrimitiveTypes::Primitive_Cube;

	size = float3(sizeX,sizeY,sizeZ);
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	// --- Vertices ---

	mesh.VerticesSize = 3 * 8 * 3;

	mesh.Vertices = new float[mesh.VerticesSize] {  // 8 of vertex coords

		sx, sy, sz,  -sx, sy, sz,  -sx,-sy, sz,   sx,-sy, sz,   // v0,v1,v2,v3 (front)
		sx, sy, sz,   sx,-sy, sz,   sx,-sy,-sz,   sx, sy,-sz,   // v0,v3,v4,v5 (right)
		sx, sy, sz,   sx, sy,-sz,  -sx, sy,-sz,  -sx, sy, sz,   // v0,v5,v6,v1 (top)
	   -sx, sy, sz,  -sx, sy,-sz,  -sx,-sy,-sz,  -sx,-sy, sz,   // v1,v6,v7,v2 (left)
	   -sx,-sy,-sz,   sx,-sy,-sz,   sx,-sy, sz,  -sx,-sy, sz,   // v7,v4,v3,v2 (bottom)
	    sx,-sy,-sz,  -sx,-sy,-sz,  -sx, sy,-sz,   sx, sy,-sz    // v4,v7,v6,v5 (back)
	};

	glGenBuffers(1, (GLuint*)&mesh.VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.VerticesSize, mesh.Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Indices ---
	mesh.IndicesSize = 3 * (2 * 6);

	mesh.Indices = new uint[mesh.IndicesSize]{ 0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
									 4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
									 8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
									12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
									16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
									20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
									};


	glGenBuffers(1, (GLuint*)&mesh.IndicesID); // create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.IndicesSize, mesh.Indices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Texture Coords ---

	mesh.TexCoordsSize = mesh.VerticesSize * 2 / 3;

	mesh.TexCoords = new float[mesh.TexCoordsSize] {
		1, 0,   0, 0,   0, 1,   1, 1,               // v0,v1,v2,v3 (front)
		0, 0,   0, 1,   1, 1,   1, 0,               // v0,v3,v4,v5 (right)
		1, 1,   1, 0,   0, 0,   0, 1,               // v0,v5,v6,v1 (top)
		1, 0,   0, 0,   0, 1,   1, 1,               // v1,v6,v7,v2 (left)
		0, 1,   1, 1,   1, 0,   0, 0,               // v7,v4,v3,v2 (bottom)
		0, 1,   1, 1,   1, 0,   0, 0                // v4,v7,v6,v5 (back)
	};

	if (checkers)
		mesh.TextureID = App->textures->GetCheckerTextureID();

	glGenBuffers(1, (GLuint*)&mesh.TextureCoordsID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.TexCoordsSize, mesh.TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer
}


// SPHERE ============================================

PrimitiveSphere::PrimitiveSphere(float size,int slices, int slacks, bool checkers)
{
	par_shapes_mesh * par_mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (par_mesh)
	{
		par_shapes_scale(par_mesh, size/2, size/2, size/2);

		mesh.IndicesSize = par_mesh->ntriangles * 3;
		mesh.VerticesSize = par_mesh->npoints * 3;

		// --- Vertices ---

		mesh.Vertices = new float[mesh.VerticesSize];

		for (uint i = 0; i < uint(par_mesh->npoints) * 3; ++i)
		{
			mesh.Vertices[i] = par_mesh->points[i];
		}

		glGenBuffers(1, (GLuint*)&mesh.VerticesID); // create buffer
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.VerticesSize, mesh.Vertices, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

		// --- Indices ---

		mesh.Indices = new unsigned[mesh.IndicesSize];
		for (uint i = 0; i < uint(par_mesh->ntriangles) * 3; ++i)
		{
			mesh.Indices[i] = par_mesh->triangles[i];
		}

		glGenBuffers(1, (GLuint*)&mesh.IndicesID); // create buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * mesh.IndicesSize, par_mesh->triangles, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

		// --- Texture Coords ---

		if (checkers)
			mesh.TextureID = App->textures->GetCheckerTextureID();

		mesh.TexCoordsSize = mesh.VerticesSize * 2;
		mesh.TexCoords = new float[mesh.TexCoordsSize];

		for (uint i = 0; i < mesh.VerticesSize; ++i)
		{
			mesh.TexCoords[2*i] = par_mesh->tcoords[2*i];
			mesh.TexCoords[(2*i) + 1] = par_mesh->tcoords[(2*i) + 1];
		}

		glGenBuffers(1, (GLuint*)&mesh.TextureCoordsID); // create buffer
		glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.TexCoordsSize, mesh.TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer


		par_shapes_free_mesh(par_mesh);

	}

}


PrimitiveSphere::PrimitiveSphere(float radius)
{
	type = type = PrimitiveTypes::Primitive_Sphere;
}

void PrimitiveSphere::InnerRender() const
{
	glColor3f(color.r, color.g, color.b);

	// --- Draw Texture ---
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
	glEnable(GL_TEXTURE_2D); // enable gl capability
	glBindTexture(GL_TEXTURE_2D, mesh.TextureID); // start using texture
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 
	glBindBuffer(GL_ARRAY_BUFFER, mesh.TextureCoordsID); // start using created buffer (tex coords)
	glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format

	// --- Draw mesh ---
	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_SHORT, NULL); // render primitives from array data

	// ----        ----

	// --- Unbind buffers, Disable capabilities ---

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)
	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); // disable client-side capability
	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should reset active texture
	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)

	glColor3f(1.0f, 1.0f, 1.0f);
}


// PLANE ==================================================


PrimitiveGrid::PrimitiveGrid()
{
	type = type = PrimitiveTypes::Primitive_Grid;
}


void PrimitiveGrid::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	float distance = 10.0f;

	for (int max_linesgrid = -distance; max_linesgrid < distance; max_linesgrid++)
	{
		glVertex3f((float)max_linesgrid, 0.0f, -distance);
		glVertex3f((float)max_linesgrid, 0.0f, distance);
		glVertex3f(-distance, 0.0f, (float)max_linesgrid);
		glVertex3f(distance, 0.0f, (float)max_linesgrid);

	}

	glVertex3f((float)-distance, 0.0f, distance);
	glVertex3f((float)distance, 0.0f, distance);
	glVertex3f((float)distance, 0.0f, -distance);
	glVertex3f((float)distance, 0.0f, distance);

	glLineWidth(1.0f);

	glEnd();

}
