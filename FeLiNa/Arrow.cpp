#include "Arrow.h"


Arrow::Arrow(float3 position, float3 size, GeometryType type) :Geometry(position, size, type) {}

void Arrow::Render() {

	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x + size.x, position.y + size.y, position.z + size.z);


	glVertex3f(position.x + size.x + 0.25f, position.y + size.y - 0.25f, position.z + size.z + 0.25f);
	glVertex3f(position.x + size.x, position.y + size.y, position.z + size.z);
	
	glVertex3f(position.x + size.x, position.y + size.y, position.z + size.z);
	glVertex3f(position.x + size.x -0.25f, position.y + size.y - 0.25f, position.z + size.z + 0.25f);

	glEnd();
}