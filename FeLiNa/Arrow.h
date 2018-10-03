#ifndef _ARROW_
#define _ARROW_


#include "Geometry.h"

class Arrow : public Geometry
{
public:

	Arrow(float3 position, float3 size, GeometryType type);

	void Render();

private:



};

#endif
