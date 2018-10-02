#include "Cube_Arrays.h"

Cube_Arrays::Cube_Arrays(float3 position, int size, GeometryType type) : Geometry(position, size, type) {

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, vertexs, GL_STATIC_DRAW);

}

void Cube_Arrays::Render() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

}
