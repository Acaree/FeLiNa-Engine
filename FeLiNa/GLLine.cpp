#include "GLLine.h"


GLLine::GLLine(float3 position, float3 size, GeometryType type) :Geometry(position, size, type) {}

void GLLine::Render() {

	glLineWidth(2.0f);
	glBegin(GL_LINES);


	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x + size.x, position.y + size.y, position.z + size.z);



	glEnd();

}



