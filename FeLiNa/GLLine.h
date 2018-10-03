#ifndef _GLLINE_
#define _GLLINE_


#include "Geometry.h"

class GLLine : public Geometry
{
public:

	GLLine(float3 position, float3 size, GeometryType type);

	void Render();

private:



};

#endif