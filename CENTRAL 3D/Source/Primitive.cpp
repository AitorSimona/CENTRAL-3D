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
}

PrimitiveSphere::PrimitiveSphere(float radius)
{
}

void PrimitiveSphere::InnerRender() const
{
}



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
