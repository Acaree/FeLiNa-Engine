#ifndef _CUBE_ARRAYS_
#define _CUBE_ARRAYS_


#include "Geometry.h"

class Cube_Arrays : public Geometry
{
public:

	Cube_Arrays(float3 position, int size, GeometryType type);

	void Render();

private:

	float vertexs[36];
	int my_id = 0;

};

#endif