#include "Cylindre.h"

Cylindre::Cylindre(float3 position, float3 size, GeometryType type) : Geometry(position, size, type) {


	index = 0;
	sindices = 1;
	position.y += latitude;
	vertex_cylinder[index] = position;

	for (uint angle = 0; angle <= 360; angle += cylinder_angle)
	{

		indices_cylinder[index] = 0;
		index++;


		if (angle == 0)
		{

			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder, position.y , position.z + sin(angle*DEGTORAD)*radius_cylinder };
			indices_cylinder[index] = sindices;
			sindices++;
			index++;

			angle += cylinder_angle;


			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder, position.y, position.z + sin(angle*DEGTORAD)*radius_cylinder };
			indices_cylinder[index] = sindices;
			sindices++;
		}
		else
		{

			indices_cylinder[index] = indices_cylinder[index - 2];
			index++;

			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder, position.y  , position.z + sin(angle*DEGTORAD)*radius_cylinder };

			indices_cylinder[index] = sindices;
			sindices++;
		}

		index++;


	}

	position.y = -latitude;
	vertex_cylinder[sindices] = position;
	int new_index_origin = sindices;
	sindices++;
	for (uint angle = 0; angle <= 360; angle += circle_angle)
	{

		indices_cylinder[index] = new_index_origin;
		index++;


		if (angle == 0)
		{

			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder, position.y , position.z + sin(angle*DEGTORAD)*radius_cylinder };
			indices_cylinder[index] = sindices;
			sindices++;
			index++;

			angle += cylinder_angle;


			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder,position.y  , position.z + sin(angle*DEGTORAD)*radius_cylinder };
			indices_cylinder[index] = sindices;
			sindices++;
		}
		else
		{

			indices_cylinder[index] = indices_cylinder[index - 2];
			index++;

			vertex_cylinder[sindices] = { position.x - cos(angle*DEGTORAD)*radius_cylinder, position.y  , position.z + sin(angle*DEGTORAD)*radius_cylinder };

			indices_cylinder[index] = sindices;
			sindices++;
		}

		index++;


	}

	int medium_lenght = index / 2;
	int j = index;

	for (int i = 1; i < j / 2; i += 3)
	{

		indices_cylinder[index] = indices_cylinder[i + 1];

		index++;
		indices_cylinder[index] = indices_cylinder[1 + i + medium_lenght];

		index++;

		indices_cylinder[index] = indices_cylinder[i];


	}

	for (int i = 1; i <= j / 2; i += 3)
	{

		indices_cylinder[index] = indices_cylinder[i + medium_lenght];

		index++;

		indices_cylinder[index] = indices_cylinder[i + 1];

		index++;

		indices_cylinder[index] = indices_cylinder[1 + i + medium_lenght];

	}

	for (int i = 1; i < j / 2; i += 3)
	{

		indices_cylinder[index] = indices_cylinder[i];

		index++;

		indices_cylinder[index] = indices_cylinder[i + medium_lenght];

		index++;

		indices_cylinder[index] = indices_cylinder[1 + i + medium_lenght];





	}


	glGenBuffers(1, (GLuint*) &(cylinder_id));
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 28 * 3, vertex_cylinder, GL_STATIC_DRAW);


	glGenBuffers(1, (GLuint*) &(cylinder_indices_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 145, indices_cylinder, GL_STATIC_DRAW);

}


void Cylindre::Render() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_indices_id);
	glDrawElements(GL_TRIANGLES, 145, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}