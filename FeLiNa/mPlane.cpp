#include "Globals.h"
#include "mPlane.h"
#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"


mPlane::mPlane(float x, float y, float z, float d) : transform(math::float4x4::identity), color(White), axis(false)
{
	math::float3 plane_pos(x, y, z);

	matGeo_plane = new math::Plane(plane_pos, d);
}

void mPlane::Render() const
{

	glPushMatrix();
	glMultMatrixf((GLfloat*)transform.ptr());

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



	InnerRender();

	glPopMatrix();
}


// ------------------------------------------------------------
void mPlane::SetPos(float x, float y, float z)
{
	transform = math::float4x4::Translate(x, y, z).ToFloat4x4() * transform;
}

// ------------------------------------------------------------
void mPlane::SetRotation(float angle, const math::float3 &u)
{
	transform = math::float4x4::RotateAxisAngle(u, angle) * transform;
}

// ------------------------------------------------------------
void mPlane::Scale(float x, float y, float z)
{
	transform = math::float4x4::Scale(x, y, z).ToFloat4x4() * transform;
}


void mPlane::InnerRender() const
{
	glLineWidth(1.0f);

	if (is_grid)
		glBegin(GL_LINES);
	else
		glBegin(GL_TRIANGLE_STRIP);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}