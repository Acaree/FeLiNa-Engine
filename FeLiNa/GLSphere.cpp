#include "GLSphere.h"

GLSphere::GLSphere(float3 position, float3 size, GeometryType type) : Geometry(position, size, type) {
	
	uint index = 0;
	uint sindices = 1;
	vertex_circle[index] = position;

	for (uint angle = 0; angle <= 360; angle += circle_angle)
	{

		indices_circle[index] = 0;
		index++;


		if (angle == 0)
		{

			vertex_circle[sindices] = { position.x + sin(angle*DEGTORAD)*radius_circle, position.y - cos(angle*DEGTORAD)*radius_circle, position.z * sin(angle*DEGTORAD)*radius_circle };
			indices_circle[sindices] = sindices;
			angle += circle_angle;
			sindices++;
			index++;

			vertex_circle[sindices] = { position.x + sin(angle*DEGTORAD)*radius_circle, position.y - cos(angle*DEGTORAD)*radius_circle, position.z * sin(angle*DEGTORAD)*radius_circle };
			indices_circle[sindices] = sindices;
			sindices++;
		}
		else
		{

			indices_circle[index] = indices_circle[index - 2];
			index++;

			vertex_circle[sindices] = { position.x + sin(angle*DEGTORAD)*radius_circle, position.y - cos(angle*DEGTORAD)*radius_circle, position.z * sin(angle*DEGTORAD)*radius_circle };

			indices_circle[index] = sindices;
			sindices++;
		}

		index++;

	}

	for (uint angle = 0; angle <= 360; angle += circle_angle)
	{

		indices_circle[index] = 0;
		index++;


		if (angle == 0)
		{
			//position.x - cos(angle*DEGTORAD)*r, position.y, position.z + sin(angle*DEGTORAD)*r
			vertex_circle[sindices] = { position.x - cos(angle*DEGTORAD)*radius_circle, position.y , position.z + sin(angle*DEGTORAD)*radius_circle };
			indices_circle[index] = sindices;
			angle += circle_angle;
			sindices++;
			index++;

			vertex_circle[sindices] = { position.x - cos(angle*DEGTORAD)*radius_circle, position.y , position.z + sin(angle*DEGTORAD)*radius_circle };
			indices_circle[index] = sindices;
			sindices++;
		}
		else
		{

			indices_circle[index] = indices_circle[index - 2];
			index++;

			vertex_circle[sindices] = { position.x - cos(angle*DEGTORAD)*radius_circle, position.y , position.z + sin(angle*DEGTORAD)*radius_circle };

			indices_circle[index] = sindices;
			sindices++;
		}

		index++;

	}


	glGenBuffers(1, (GLuint*) &(sphere_id));
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 27 * 3, vertex_circle, GL_STATIC_DRAW);


	glGenBuffers(1, (GLuint*) &(sphere_indices_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 72, indices_circle, GL_STATIC_DRAW);
}


void GLSphere::Render() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_id);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}