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

	InnerRender();

}


// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.Translate(x, y, z);
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
	glBegin(GL_TRIANGLES);  // draw a cube with 12 triangles

	// front face =================
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(size.x,0.0f,0.0f);
	glVertex3f(0.0f,size.y,0.0f);

	glVertex3f(size.x, 0.0f, 0.0f);
	glVertex3f(size.x, size.y, 0.0f);
	glVertex3f(0.0f, size.y, 0.0f);

	// right face =================
	glVertex3f(0.0f, 0.0f, -size.z);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, size.y, 0.0f);

	glVertex3f(0.0f, 0.0f, -size.z);
	glVertex3f(0.0f, size.y, 0.0f);
	glVertex3f(0.0f, size.y, -size.z);

	// top face ===================
	glVertex3f(0.0f, size.y, -size.z);
	glVertex3f(0.0f, size.y, 0.0f);
	glVertex3f(size.x, size.y, 0.0f);

	glVertex3f(0.0f, size.y, -size.z);
	glVertex3f(size.x, size.y, 0.0f);
	glVertex3f(size.x, size.y, -size.z);

	//// back face =================
	glVertex3f(0.0f, size.y, -size.z);
	glVertex3f(size.x, 0.0f, -size.z);
	glVertex3f(0.0f, 0.0f, -size.z);

	glVertex3f(0.0f, size.y, -size.z);
	glVertex3f(size.x, size.y, -size.z);
	glVertex3f(size.x, 0.0f, -size.z);

	//// left face =================
	glVertex3f(size.x, size.y, 0.0f);
	glVertex3f(size.x, 0.0f, 0.0f);
	glVertex3f(size.x, 0.0f, -size.z);

	glVertex3f(size.x, size.y, -size.z);
	glVertex3f(size.x, size.y, 0.0f);
	glVertex3f(size.x, 0.0f, -size.z);

	//// bottom face ===================
	glVertex3f(size.x, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -size.z);

	glVertex3f(size.x, 0.0f, -size.z);
	glVertex3f(size.x, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -size.z);


	glEnd();
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
