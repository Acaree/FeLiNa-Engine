#include "Cube_ArraysandIndices.h"

Cube_ArraysandIndices::Cube_ArraysandIndices(float3 position, int size, GeometryType type) : Geometry(position, size, type) {

	vertexs[0] = 1.0f;
	vertexs[1] = 1.0f;
	vertexs[2] = 0.0f;
	vertexs[3] = 0.0f;
	vertexs[4] = 1.0f;
	vertexs[5] = 0.0f;
	vertexs[6] = 1.0f;
	vertexs[7] = 1.0f;
	vertexs[8] = 1.0f;
	
	glGenBuffers(1, (GLuint*) &(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertexs, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 180 * 3, vertexs, GL_STATIC_DRAW);


	glGenBuffers(1, (GLuint*) &(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 8, indices, GL_STATIC_DRAW);

}

void Cube_ArraysandIndices::Render() {

	

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);



}
