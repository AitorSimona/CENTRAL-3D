#include "Globals.h"
#include "OpenGL.h"
#include "Primitive.h"
#include "par/par_shapes.h"
#include "Application.h"
#include "ModuleRenderer3D.h"


// ------------------------------------------------------------
Primitive::Primitive() : transform(math::float4x4::identity), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Cube)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf((math::float4x4::identity* this->transform).Transposed().ptr());

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

	glColor3f(color.r, color.g, color.b);

	if (wire || App->renderer3D->wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

	InnerRender();

	if (wire || App->renderer3D->wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	glPopMatrix();
}


// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.TransformPos(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const float3 &u)
{
	transform.RotateAxisAngle(u, angle);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.Scale(x, y, z);
}

// CUBE ============================================

PrimitiveCube::PrimitiveCube()
{

}

PrimitiveCube::PrimitiveCube(float sizeX, float sizeY, float sizeZ)
{

}

void PrimitiveCube::InnerRender() const
{

	bool direct_mode = true;
	bool vertex_arrays = false;
	bool indices = false;

	if (direct_mode)
	{
		glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles

		// front face =================
		glVertex3f(-size.x, -size.y, size.z);
		glVertex3f(size.x, -size.y, size.z);
		glVertex3f(size.x, size.y, size.z);

		glVertex3f(size.x, size.y, size.z);
		glVertex3f(-size.x, size.y, size.z);
		glVertex3f(-size.x, -size.y, size.z);

		// right face =================
		glVertex3f(size.x, size.y, size.z);
		glVertex3f(size.x, -size.y, size.z);
		glVertex3f(size.x, size.y, -size.z);

		glVertex3f(size.x, -size.y, size.z);
		glVertex3f(size.x, -size.y, -size.z);
		glVertex3f(size.x, size.y, -size.z);

		// top face ===================
		glVertex3f(-size.x, size.y, -size.z);
		glVertex3f(-size.x, size.y, size.z);
		glVertex3f(size.x, size.y, size.z);

		glVertex3f(size.x, size.y, size.z);
		glVertex3f(size.x, size.y, -size.z);
		glVertex3f(-size.x, size.y, -size.z);

		////// back face =================
		glVertex3f(size.x, size.y, -size.z);
		glVertex3f(size.x, -size.y, -size.z);
		glVertex3f(-size.x, -size.y, -size.z);

		glVertex3f(-size.x, -size.y, -size.z);
		glVertex3f(-size.x, size.y, -size.z);
		glVertex3f(size.x, size.y, -size.z);

		////// left face =================
		glVertex3f(-size.x, size.y, -size.z);
		glVertex3f(-size.x, -size.y, size.z);
		glVertex3f(-size.x, size.y, size.z);

		glVertex3f(-size.x, size.y, -size.z);
		glVertex3f(-size.x, -size.y, -size.z);
		glVertex3f(-size.x, -size.y, size.z);

		////// bottom face ===================

		glVertex3f(size.x, -size.y, size.z);
		glVertex3f(-size.x, -size.y, size.z);
		glVertex3f(-size.x, -size.y, -size.z);

		glVertex3f(-size.x, -size.y, -size.z);
		glVertex3f(size.x, -size.y, -size.z);
		glVertex3f(size.x, -size.y, size.z);

		glEnd();
	}

	else if (vertex_arrays)
	{
		GLfloat vertices[36*3] = {
	-size.x,-size.y,-size.z,
	-size.x,-size.y, size.z,
	-size.x, size.y, size.z,
	size.x, size.y,-size.z,
	-size.x,-size.y,-size.z,
	-size.x, size.y,-size.z,
	size.x,-size.y, size.z,
	-size.x,-size.y,-size.z,
	size.x,-size.y,-size.z,
	size.x, size.y,-size.z,
	size.x,-size.y,-size.z,
	-size.x,-size.y,-size.z,
	-size.x,-size.y,-size.z,
	-size.x, size.y, size.z,
	-size.x, size.y,-size.z,
	size.x,-size.y, size.z,
	-size.x,-size.y, size.z,
	-size.x,-size.y,-size.z,
	-size.x, size.y, size.z,
	-size.x,-size.y, size.z,
	size.x,-size.y, size.z,
	size.x, size.y, size.z,
	size.x,-size.y,-size.z,
	size.x, size.y,-size.z,
	size.x,-size.y,-size.z,
	size.x, size.y, size.z,
	size.x,-size.y, size.z,
	size.x, size.y, size.z,
	size.x, size.y,-size.z,
	-size.x, size.y,-size.z,
	size.x, size.y, size.z,
	-size.x, size.y,-size.z,
	-size.x, size.y, size.z,
	size.x, size.y, size.z,
	-size.x, size.y, size.z,
	size.x,-size.y, size.z
	};


		// activate and specify pointer to vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);

		// draw a cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// deactivate vertex arrays after drawing
		glDisableClientState(GL_VERTEX_ARRAY);

	}

	else if (indices)
	{
			GLfloat vertices[8 * 3] = {  // 8 of vertex coords

		-size.x, size.y, size.z,
		-size.x,-size.y,size.z,
		size.x, -size.y,size.z,
		size.x, size.y, size.z,

		
		size.x, size.y, -size.z,
		-size.x, size.y,-size.z,
		-size.x,-size.y, -size.z,
		size.x, -size.y,-size.z,

		};

		GLubyte indices[] = { 0,1,2, 2,3,0,   // 36 of indices
								 0,3,4, 4,5,0,
								 0,5,6, 6,1,0,
								 1,6,7, 7,2,1,
								 7,4,3, 3,2,7,
								 4,7,6, 6,5,4 };


	// activate and specify pointer to vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	// draw a cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	}

}


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

// CYLINDER ============================================



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
