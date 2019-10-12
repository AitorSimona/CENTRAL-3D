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

#define CHECKERS_WIDTH 64
#define CHECKERS_HEIGHT 64

PrimitiveCube::PrimitiveCube(float sizeX, float sizeY, float sizeZ)
{
	type = type = PrimitiveTypes::Primitive_Cube;

	size = float3(sizeX,sizeY,sizeZ);
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	// --- Vertices ---

	uint verticesSize = 3 * 8;

	Vertices = new float[verticesSize] {  // 8 of vertex coords

		    sx, sy, sz,
		   -sx, sy, sz,
           -sx,-sy, sz,
			sx,-sy, sz,
					
			sx,-sy,-sz,
		    sx, sy,-sz,
		   -sx, sy,-sz,
		   -sx,-sy,-sz,
	};


	glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Indices ---
	IndicesSize = 3 * (2 * 6);
	Indices = new uint[IndicesSize]{ 0,1,2, 2,3,0, // front face 36 of indices
							 0,3,4, 4,5,0,		    // right face
							 0,5,6, 6,1,0,		    // top face
							 1,6,7, 7,2,1,		    // left face
							 7,4,3, 3,2,7,		    // bottom face
							 4,7,6, 6,5,4 };	   // back face


	glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer


	//// --- Checkers Texture ---
	//GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	//for (int i = 0; i < CHECKERS_HEIGHT; i++) {
	//	for (int j = 0; j < CHECKERS_WIDTH; j++) {
	//		int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
	//		checkImage[i][j][0] = (GLubyte)c;
	//		checkImage[i][j][1] = (GLubyte)c;
	//		checkImage[i][j][2] = (GLubyte)c;
	//		checkImage[i][j][3] = (GLubyte)255;
	//	}
	//}

	//// texCoord array
	//texCoords = new  float[IndicesSize*2] { 1, 0,   0, 0,   0, 1,   0, 1,   1, 1,  1, 0,           // v0,v1,v2,v2,v3,v0 (front)
	//									  0, 0,   0, 1,   1, 1,   1, 1,   1, 0,  0, 0,           // v0,v3,v4,v4,v5,v0 (right)
	//									  1, 1,   1, 0,   0, 0,   0, 0,   0, 1,  1, 1,           // v0,v5,v6,v6,v1,v0 (top)
	//									  1, 0,   0, 0,   0, 1,   0, 1,   1, 1,  1, 0,           // v1,v6,v7,v7,v2,v1 (left)
	//									  0, 1,   1, 1,   1, 0,   1, 0,   0, 0,  0, 1,           // v7,v4,v3,v3,v2,v7 (bottom)
	//									  0, 1,   1, 1,   1, 0,   1, 0,   0, 0,  0, 1 };         // v4,v7,v6,v6,v5,v4 (back)

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//glGenTextures(1, &texID);
	//glBindTexture(GL_TEXTURE_2D, texID);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
	//	0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}

//void PrimitiveCube::InnerRender() const
//{
//
//	bool direct_mode = false;
//	bool vertex_arrays = false;
//	bool indices = true;
//
//	//if (direct_mode)
//	//{
//	//	glEnable(GL_TEXTURE_2D);
//	//	glBindTexture(GL_TEXTURE_2D, 0);
//	//	glBindTexture(GL_TEXTURE_2D, this->texID);
//
//	//	glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles
//
//	//	// front face =================
//	//	glVertex3f(-size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(size.x, -size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(size.x, size.y, size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//
//	//	glVertex3f(size.x, size.y, size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//	//	glVertex3f(-size.x, size.y, size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(-size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//
//	//	// right face =================
//	//	glVertex3f(size.x, size.y, size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(size.x, size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//
//	//	glVertex3f(size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(size.x, -size.y, -size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(size.x, size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//
//	//	// top face ===================
//	//	glVertex3f(-size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(-size.x, size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(size.x, size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//
//
//	//	glVertex3f(size.x, size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(size.x, size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//	//	glVertex3f(-size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//
//	//	////// back face =================
//	//	glVertex3f(size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(size.x, -size.y, -size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(-size.x, -size.y, -size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//
//	//	glVertex3f(-size.x, -size.y, -size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(-size.x, size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//	//	glVertex3f(size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//
//	//	////// left face =================
//	//	glVertex3f(-size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(-size.x, -size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(-size.x, size.y, size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//
//	//	glVertex3f(-size.x, size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(-size.x, -size.y, -size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(-size.x, -size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//
//	//	////// bottom face ===================
//
//	//	glVertex3f(size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//	//	glVertex3f(-size.x, -size.y, size.z);
//	//	glTexCoord2f(1.0f, 0.0f);
//	//	glVertex3f(-size.x, -size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//
//	//	glVertex3f(-size.x, -size.y, -size.z);
//	//	glTexCoord2f(1.0f, 1.0f);
//	//	glVertex3f(size.x, -size.y, -size.z);
//	//	glTexCoord2f(0.0f, 1.0f);
//	//	glVertex3f(size.x, -size.y, size.z);
//	//	glTexCoord2f(0.0f, 0.0f);
//
//
//	//	glEnd();
//	//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	//}
//
//	//else if (vertex_arrays)
//	//{
//	//	GLfloat vertices[36*3] = {
//	//-size.x,-size.y,-size.z,
//	//-size.x,-size.y, size.z,
//	//-size.x, size.y, size.z,
//	//size.x, size.y,-size.z,
//	//-size.x,-size.y,-size.z,
//	//-size.x, size.y,-size.z,
//	//size.x,-size.y, size.z,
//	//-size.x,-size.y,-size.z,
//	//size.x,-size.y,-size.z,
//	//size.x, size.y,-size.z,
//	//size.x,-size.y,-size.z,
//	//-size.x,-size.y,-size.z,
//	//-size.x,-size.y,-size.z,
//	//-size.x, size.y, size.z,
//	//-size.x, size.y,-size.z,
//	//size.x,-size.y, size.z,
//	//-size.x,-size.y, size.z,
//	//-size.x,-size.y,-size.z,
//	//-size.x, size.y, size.z,
//	//-size.x,-size.y, size.z,
//	//size.x,-size.y, size.z,
//	//size.x, size.y, size.z,
//	//size.x,-size.y,-size.z,
//	//size.x, size.y,-size.z,
//	//size.x,-size.y,-size.z,
//	//size.x, size.y, size.z,
//	//size.x,-size.y, size.z,
//	//size.x, size.y, size.z,
//	//size.x, size.y,-size.z,
//	//-size.x, size.y,-size.z,
//	//size.x, size.y, size.z,
//	//-size.x, size.y,-size.z,
//	//-size.x, size.y, size.z,
//	//size.x, size.y, size.z,
//	//-size.x, size.y, size.z,
//	//size.x,-size.y, size.z
//	//};
//
//
//	//	// activate and specify pointer to vertex array
//	//	glEnableClientState(GL_VERTEX_ARRAY);
//	//	glVertexPointer(3, GL_FLOAT, 0, vertices);
//
//	//	// draw a cube
//	//	glDrawArrays(GL_TRIANGLES, 0, 36);
//
//	//	// deactivate vertex arrays after drawing
//	//	glDisableClientState(GL_VERTEX_ARRAY);
//
//	//}
//
//	//else if (indices)
//	//{
//	//	//	GLfloat vertices[8 * 3] = {  // 8 of vertex coords
//	//	//size.x, size.y, size.z,
//	//	//-size.x, size.y, size.z,
//	//	//-size.x,-size.y,size.z,
//	//	//size.x, -size.y,size.z,	
//
//	//	//size.x,-size.y, -size.z,
//	//	//size.x, size.y,-size.z,
//	// //  -size.x, size.y, -size.z,
//	// //  -size.x,-size.y,-size.z
//	//	//};
//
//	//	
//	//		//GLfloat vertices[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
//	//		//		   1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
//	//		//		   1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
//	//		//		  -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
//	//		//		  -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
//	//		//		   1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)
//
//	//	
//
//	//	//GLubyte indices[] = { 0,1,2, 2,3,0,   // front face 36 of indices
//	//	//						 0,3,4, 4,5,0, // right face
//	//	//						 0,5,6, 6,1,0, // top face
//	//	//						 1,6,7, 7,2,1, // left face
//	//	//						 7,4,3, 3,2,7, // bottom face
//	//	//						 4,7,6, 6,5,4 }; // back face
//
//	//		//// index array of vertex array for glDrawElements() & glDrawRangeElement()
//	//		//GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
//	//		//					   4, 5, 6,   6, 7, 4,      // right
//	//		//					   8, 9,10,  10,11, 8,      // top
//	//		//					  12,13,14,  14,15,12,      // left
//	//		//					  16,17,18,  18,19,16,      // bottom
//	//		//					  20,21,22,  22,23,20 };    // back
//
//
//	//// texCoord array
//	////GLfloat texCoords [] =				{ 1, 0,   0, 0,   0, 1,   0, 1,   1, 1,  1, 0,           // v0,v1,v2,v2,v3,v0 (front)
//	////									  0, 0,   0, 1,   1, 1,   1, 1,   1, 0,  0, 0,           // v0,v3,v4,v4,v5,v0 (right)
//	////									  1, 1,   1, 0,   0, 0,   0, 0,   0, 1,  1, 1,           // v0,v5,v6,v6,v1,v0 (top)
//	////									  1, 0,   0, 0,   0, 1,   0, 1,   1, 1,  1, 0,           // v1,v6,v7,v7,v2,v1 (left)
//	////									  0, 1,   1, 1,   1, 0,   1, 0,   0, 0,  0, 1,           // v7,v4,v3,v3,v2,v7 (bottom)
//	////									  0, 1,   1, 1,   1, 0,   1, 0,   0, 0,  0, 1 };         // v4,v7,v6,v6,v5,v4 (back)
//
//	//glEnable(GL_TEXTURE_2D);
//
//	////// activate and specify pointer to vertex array
//	////glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	////glEnableClientState(GL_VERTEX_ARRAY);
//
//	////glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
//	////glVertexPointer(3, GL_FLOAT, 0, vertices);
//
//	////// draw a cube
//	////glBindTexture(GL_TEXTURE_2D, this->texID);
//	////glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
//
//
//	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	//glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
//
//	//glBindTexture(GL_TEXTURE_2D, this->texID);
//	//glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
//
//	//glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer (vertices)
//	//glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
//
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer (indices)
//	//glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data
//
//	//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)
//	//glBindTexture(GL_TEXTURE_2D, 0);
//
//	//glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
//	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	//}
//
//}


// SPHERE ============================================

PrimitiveSphere::PrimitiveSphere(float size,int slices, int slacks)
{
	par_shapes_mesh * mesh = par_shapes_create_parametric_sphere(slices, slacks);

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		IndicesSize = mesh->ntriangles * 3;
		verticesSize = mesh->npoints * 3;

		Vertices = new float[verticesSize];

		for (uint i = 0; i < uint(mesh->npoints) * 3; ++i)
		{
			Vertices[i] = mesh->points[i];
		}


		Indices = new unsigned[IndicesSize];
		for (uint i = 0; i < uint(mesh->ntriangles) * 3; ++i)
		{
			Indices[i] = mesh->triangles[i];
		}

		glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
		glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

		// --- Indices ---

		glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T) * IndicesSize, mesh->triangles, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer


		par_shapes_free_mesh(mesh);

	}

}


PrimitiveSphere::PrimitiveSphere(float radius)
{
	type = type = PrimitiveTypes::Primitive_Sphere;
}

void PrimitiveSphere::InnerRender() const
{
	glColor3f(color.r, color.g, color.b);

	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability

	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_SHORT, NULL); // render primitives from array data

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability

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
