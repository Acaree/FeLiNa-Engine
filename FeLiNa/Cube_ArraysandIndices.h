#ifndef _CUBE_ARRAYSANDINDICES_
#define _CUBE_ARRAYSANDINDICES_


#include "Geometry.h"

class Cube_ArraysandIndices : public Geometry
{
public:

	Cube_ArraysandIndices(float3 position, int size, GeometryType type);

	void Render();

private:

	float vertexs[36];
	int indices[8] = {0,1,2};
	
	int my_id = 0;
	int my_indices = 0;
};

#endif
