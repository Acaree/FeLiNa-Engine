#include "Cube_Arrays.h"

Cube_Arrays::Cube_Arrays(float3 position, int size, GeometryType type) : Geometry(position, size, type) {


	for (int i = 0; i < 108; i += 3) {

		vertexs[i] += position.x;
		vertexs[i + 1] += position.y;
		vertexs[i + 2] += position.z;
	}
	
	glGenBuffers(1, (GLuint*) &(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertexs, GL_STATIC_DRAW);

}

void Cube_Arrays::Render() {

	
	glColor3f(0.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
