#ifndef _CUBE_DIRECTMODE_
#define _CUBE_DIRECTMODE_


#include "Geometry.h"

class Cube_DirectMode : public Geometry
{
public:

	Cube_DirectMode(float3 position, int size, GeometryType type);

	void Render();

private:


	
};

#endif
