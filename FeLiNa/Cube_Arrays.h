#ifndef _CUBE_ARRAYS_
#define _CUBE_ARRAYS_


#include "Geometry.h"

class Cube_Arrays : public Geometry
{
public:

	Cube_Arrays(float3 position, float3 size, GeometryType type);

	void Render();

private:

	
	float vertexs[108] = {	0.0f,1.0f,0.0f,		0.0f,1.0f,1.0f,		1.0f,1.0f,1.0f,
							1.0f,1.0f,0.0f,		0.0f,1.0f,0.0f,		1.0f,1.0f,1.0f, 
							0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f, 1.0f,
							1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f,
							1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,
							1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f,
							0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
							1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
							1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f
	};
	uint my_id = 0;

};

#endif