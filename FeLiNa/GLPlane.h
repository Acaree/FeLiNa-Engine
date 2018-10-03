#ifndef _GLPLANE_
#define _GLPLANE_


#include "Geometry.h"

class GLPlane : public Geometry
{
public:

	GLPlane(float3 position, float3 size, GeometryType type);

	void Render();

private:

	float vertexs[12] = { 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		

	};
	uint indices[6] = {1,0,2,1,2,3};

	uint my_id = 0;
	uint my_indices = 0;

	bool is_axis;
};

#endif
