#ifndef _CUBE_ARRAYSANDINDEX_
#define _CUBE_ARRAYSANDINDEX_


#include "Geometry.h"

class Cube_ArraysandIndex : public Geometry
{
public:

	Cube_ArraysandIndex(float3 position, int size, GeometryType type);

	void Render();

private:

	float vertexs[24] = { 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,

	};
	uint indices[36] = {6,2,7,
						2,3,7,
						3,1,5,
						7,3,5,
						4,6,7,
						4,7,5,
						0,5,1,
						4,5,0,
						1,3,2,
						0,1,2,
						0,2,4,
						4,2,6};
	
	uint my_id = 0;
	uint my_indices = 0;
};

#endif
