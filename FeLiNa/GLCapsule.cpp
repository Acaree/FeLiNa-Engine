#include "GLCapsule.h"


GLCapsule::GLCapsule(float3 position, float3 size, GeometryType type) : Geometry(position, size, type) {

	uint index = 0;
	uint sindices = 1;
	position.y += latitude_capsule;
	vertex_capsule[index] = position;

	for (uint angle = 0; angle <= 360; angle += capsule_angle)
	{

		indices_capsule[index] = 0;
		index++;


		if (angle == 0)
		{

			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y - latitude_capsule, position.z + sin(angle*DEGTORAD)*radius_capsule };
			indices_capsule[index] = sindices;
			sindices++;
			index++;

			angle += capsule_angle;


			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y - latitude_capsule , position.z + sin(angle*DEGTORAD)*radius_capsule };
			indices_capsule[index] = sindices;
			sindices++;
		}
		else
		{

			indices_capsule[index] = indices_capsule[index - 2];
			index++;

			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y - latitude_capsule, position.z + sin(angle*DEGTORAD)*radius_capsule };

			indices_capsule[index] = sindices;
			sindices++;
		}

		index++;


	}

	position.y = -latitude_capsule;
	vertex_capsule[sindices] = position;
	uint new_index_position = sindices;
	sindices++;
	for (uint angle = 0; angle <= 360; angle += capsule_angle)
	{

		indices_capsule[index] = new_index_position;
		index++;


		if (angle == 0)
		{

			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y + latitude_capsule, position.z + sin(angle*DEGTORAD)*radius_capsule };
			indices_capsule[index] = sindices;
			sindices++;
			index++;

			angle += capsule_angle;


			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y + latitude_capsule , position.z + sin(angle*DEGTORAD)*radius_capsule };
			indices_capsule[index] = sindices;
			sindices++;
		}
		else
		{

			indices_capsule[index] = indices_capsule[index - 2];
			index++;

			vertex_capsule[sindices] = { position.x - cos(angle*DEGTORAD)*radius_capsule, position.y + latitude_capsule , position.z + sin(angle*DEGTORAD)*radius_capsule };

			indices_capsule[index] = sindices;
			sindices++;
		}

		index++;


	}

	uint medium_lenght = index / 2;
	int j = index;

	for (int i = 1; i < j / 2; i += 3)
	{

		indices_capsule[index] = indices_capsule[i + 1];

		index++;
		indices_capsule[index] = indices_capsule[1 + i + medium_lenght];

		index++;

		indices_capsule[index] = indices_capsule[i];


	}

	for (int i = 1; i <= j / 2; i += 3)
	{

		indices_capsule[index] = indices_capsule[i + medium_lenght];

		index++;

		indices_capsule[index] = indices_capsule[i + 1];

		index++;

		indices_capsule[index] = indices_capsule[1 + i + medium_lenght];

	}

	for (int i = 1; i < j / 2; i += 3)
	{

		indices_capsule[index] = indices_capsule[i];

		index++;

		indices_capsule[index] = indices_capsule[i + medium_lenght];

		index++;

		indices_capsule[index] = indices_capsule[1 + i + medium_lenght];





	}


	glGenBuffers(1, (GLuint*) &(capsule_id));
	glBindBuffer(GL_ARRAY_BUFFER, capsule_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 28 * 3, vertex_capsule, GL_STATIC_DRAW);


	glGenBuffers(1, (GLuint*) &(capsule_indices_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, capsule_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 145, indices_capsule, GL_STATIC_DRAW);

}



void GLCapsule::Render() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, capsule_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, capsule_indices_id);
	glDrawElements(GL_TRIANGLES, 145, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	
}