#include "Globals.h"
#include "OpenGL.h"
#include "Primitive.h"
#include "par/par_shapes.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "mmgr/mmgr.h"

// ------------------------------------------------------------
Primitive::Primitive() : transform(math::float4x4::identity), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Cube)
{}

Primitive::~Primitive()
{
	glDeleteBuffers(1, (GLuint*)&VerticesID);
	glDeleteBuffers(1, (GLuint*)&IndicesID);

	RELEASE_ARRAY(Vertices);
	RELEASE_ARRAY(Indices);
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

	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability

	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability

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

PrimitiveCube::PrimitiveCube(float sizeX, float sizeY, float sizeZ)
{
	size = float3(sizeX,sizeY,sizeZ);
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	// --- Vertices ---

	uint verticesSize = 3 * 8;

	Vertices = new float[verticesSize] {  // 8 of vertex coords

		   -sx, sy, sz,
		   -sx,-sy, sz,
		    sx,-sy, sz,
			sx, sy, sz,
					
			sx, sy,-sz,
		   -sx, sy,-sz,
		   -sx,-sy,-sz,
			sx,-sy,-sz,
	};

	glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Indices ---
	IndicesSize = 3 * (2 * 6);
	Indices = new uint[IndicesSize]{ 0,1,2, 2,3,0,   // 36 of indices
							 0,3,4, 4,5,0,
							 0,5,6, 6,1,0,
							 1,6,7, 7,2,1,
							 7,4,3, 3,2,7,
							 4,7,6, 6,5,4 };



	glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer
}

//void PrimitiveCube::InnerRender() const
//{
//
//	bool direct_mode = true;
//	bool vertex_arrays = false;
//	bool indices = false;
//
//	if (direct_mode)
//	{
//		glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles
//
//		// front face =================
//		glVertex3f(-size.x, -size.y, size.z);
//		glVertex3f(size.x, -size.y, size.z);
//		glVertex3f(size.x, size.y, size.z);
//
//		glVertex3f(size.x, size.y, size.z);
//		glVertex3f(-size.x, size.y, size.z);
//		glVertex3f(-size.x, -size.y, size.z);
//
//		// right face =================
//		glVertex3f(size.x, size.y, size.z);
//		glVertex3f(size.x, -size.y, size.z);
//		glVertex3f(size.x, size.y, -size.z);
//
//		glVertex3f(size.x, -size.y, size.z);
//		glVertex3f(size.x, -size.y, -size.z);
//		glVertex3f(size.x, size.y, -size.z);
//
//		// top face ===================
//		glVertex3f(-size.x, size.y, -size.z);
//		glVertex3f(-size.x, size.y, size.z);
//		glVertex3f(size.x, size.y, size.z);
//
//		glVertex3f(size.x, size.y, size.z);
//		glVertex3f(size.x, size.y, -size.z);
//		glVertex3f(-size.x, size.y, -size.z);
//
//		////// back face =================
//		glVertex3f(size.x, size.y, -size.z);
//		glVertex3f(size.x, -size.y, -size.z);
//		glVertex3f(-size.x, -size.y, -size.z);
//
//		glVertex3f(-size.x, -size.y, -size.z);
//		glVertex3f(-size.x, size.y, -size.z);
//		glVertex3f(size.x, size.y, -size.z);
//
//		////// left face =================
//		glVertex3f(-size.x, size.y, -size.z);
//		glVertex3f(-size.x, -size.y, size.z);
//		glVertex3f(-size.x, size.y, size.z);
//
//		glVertex3f(-size.x, size.y, -size.z);
//		glVertex3f(-size.x, -size.y, -size.z);
//		glVertex3f(-size.x, -size.y, size.z);
//
//		////// bottom face ===================
//
//		glVertex3f(size.x, -size.y, size.z);
//		glVertex3f(-size.x, -size.y, size.z);
//		glVertex3f(-size.x, -size.y, -size.z);
//
//		glVertex3f(-size.x, -size.y, -size.z);
//		glVertex3f(size.x, -size.y, -size.z);
//		glVertex3f(size.x, -size.y, size.z);
//
//		glEnd();
//	}
//
//	else if (vertex_arrays)
//	{
//		GLfloat vertices[36*3] = {
//	-size.x,-size.y,-size.z,
//	-size.x,-size.y, size.z,
//	-size.x, size.y, size.z,
//	size.x, size.y,-size.z,
//	-size.x,-size.y,-size.z,
//	-size.x, size.y,-size.z,
//	size.x,-size.y, size.z,
//	-size.x,-size.y,-size.z,
//	size.x,-size.y,-size.z,
//	size.x, size.y,-size.z,
//	size.x,-size.y,-size.z,
//	-size.x,-size.y,-size.z,
//	-size.x,-size.y,-size.z,
//	-size.x, size.y, size.z,
//	-size.x, size.y,-size.z,
//	size.x,-size.y, size.z,
//	-size.x,-size.y, size.z,
//	-size.x,-size.y,-size.z,
//	-size.x, size.y, size.z,
//	-size.x,-size.y, size.z,
//	size.x,-size.y, size.z,
//	size.x, size.y, size.z,
//	size.x,-size.y,-size.z,
//	size.x, size.y,-size.z,
//	size.x,-size.y,-size.z,
//	size.x, size.y, size.z,
//	size.x,-size.y, size.z,
//	size.x, size.y, size.z,
//	size.x, size.y,-size.z,
//	-size.x, size.y,-size.z,
//	size.x, size.y, size.z,
//	-size.x, size.y,-size.z,
//	-size.x, size.y, size.z,
//	size.x, size.y, size.z,
//	-size.x, size.y, size.z,
//	size.x,-size.y, size.z
//	};
//
//
//		// activate and specify pointer to vertex array
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glVertexPointer(3, GL_FLOAT, 0, vertices);
//
//		// draw a cube
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		// deactivate vertex arrays after drawing
//		glDisableClientState(GL_VERTEX_ARRAY);
//
//	}
//
//	else if (indices)
//	{
//			GLfloat vertices[8 * 3] = {  // 8 of vertex coords
//
//		-size.x, size.y, size.z,
//		-size.x,-size.y,size.z,
//		size.x, -size.y,size.z,
//		size.x, size.y, size.z,
//
//		
//		size.x, size.y, -size.z,
//		-size.x, size.y,-size.z,
//		-size.x,-size.y, -size.z,
//		size.x, -size.y,-size.z,
//
//		};
//
//		GLubyte indices[] = { 0,1,2, 2,3,0,   // 36 of indices
//								 0,3,4, 4,5,0,
//								 0,5,6, 6,1,0,
//								 1,6,7, 7,2,1,
//								 7,4,3, 3,2,7,
//								 4,7,6, 6,5,4 };
//
//
//	// activate and specify pointer to vertex array
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, vertices);
//
//	// draw a cube
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
//
//	// deactivate vertex arrays after drawing
//	glDisableClientState(GL_VERTEX_ARRAY);
//	}
//
//}


// SPHERE ============================================

PrimitiveSphere::PrimitiveSphere()
{
	//par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(4,5);

	//IndicesSize = mesh->ntriangles * 3;
	//verticesSize = mesh->npoints*3;

	//Vertices = new float[verticesSize];
	//Vertices = mesh->points;

	///*Indices = new uint[IndicesSize];
	//Indices = mesh->triangles;*/

	//glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
	//glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	//// --- Indices ---

	//glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * IndicesSize, mesh->triangles, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer


	//par_shapes_free_mesh(mesh);

	//uint rings = 12;
	//uint sectors = 24;
	//float radius = 1;

	//float const R = 1. / (float)(rings - 1);
	//float const S = 1. / (float)(sectors - 1);
	//int r, s;

	//// --- Vertices ---

	//verticesSize = rings * sectors * 3;
	//Vertices = new float[verticesSize];

	//uint iterator = 0;

	//for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
	//	float const y = sin(-M_PI_2 + M_PI * r * R);
	//	float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
	//	float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

	//	Vertices[iterator++] = x * radius;
	//	Vertices[iterator++] = y * radius;
	//	Vertices[iterator++] = z * radius;
	//}

	//glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
 //   glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
 //   glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	//// --- Indices ---

	//iterator = 0;

	//IndicesSize = rings * sectors * 4;
	//Indices = new uint[IndicesSize];
	//
	//for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
	//	Indices[iterator++] = r * sectors + s;
	//	Indices[iterator++] = r * sectors + (s + 1);
	//	Indices[iterator++] = (r + 1) * sectors + (s + 1);
	//	Indices[iterator++] = (r + 1) * sectors + s;
	//}

	//glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer



	//int stacks = 20;
	//int slices = 20;
	//const float PI = 3.14f;

	//std::vector<float> positions;
	//std::vector<GLuint> indices;

	//// loop through stacks.
	//for (int i = 0; i <= stacks; ++i) {

	//	float V = (float)i / (float)stacks;
	//	float phi = V * PI;

	//	// loop through the slices.
	//	for (int j = 0; j <= slices; ++j) {

	//		float U = (float)j / (float)slices;
	//		float theta = U * (PI * 2);

	//		// use spherical coordinates to calculate the positions.
	//		float x = cos(theta) * sin(phi);
	//		float y = cos(phi);
	//		float z = sin(theta) * sin(phi);

	//		positions.push_back(x);
	//		positions.push_back(y);
	//		positions.push_back(z);
	//	}

	//}
	//verticesSize = positions.size();
	//Vertices = new float[verticesSize];


	//for (int i = 0; i < positions.size(); ++i)
	//{
	//	Vertices[i] = positions[i];
	//}

	//glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
	//glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	//// Calc The Index Positions
	//for (int i = 0; i < slices * stacks + slices; ++i) {
	//	indices.push_back(GLuint(i));
	//	indices.push_back(GLuint(i + slices + 1));
	//	indices.push_back(GLuint(i + slices));

	//	indices.push_back(GLuint(i + slices + 1));
	//	indices.push_back(GLuint(i));
	//	indices.push_back(GLuint(i + 1));
	//}

	//IndicesSize = indices.size();
	//Indices = new uint[IndicesSize];

	//for (int i = 0; i < indices.size(); ++i)
	//{
	//	Indices[i] = indices[i];
	//}

	//glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer
}



PrimitiveSphere::PrimitiveSphere(float radius)
{
}

//void PrimitiveSphere::InnerRender() const
//{
//	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
//	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer (vertices)
//	glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (indices)
//	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
//}



// PLANE ==================================================


PrimitivePlane::PrimitivePlane()
{
}

PrimitivePlane::PrimitivePlane(float x, float y, float z, float d)
{
}

void PrimitivePlane::InnerRender() const
{
}
