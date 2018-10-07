#ifndef _CAPSULE_
#define _CAPSULE_


#include "Geometry.h"

class GLCapsule : public Geometry
{
public:

	GLCapsule(float3 position, float3 size, GeometryType type);

	void Render();

private:

	float capsule_angle = 30.F;
	uint capsule_id = 0;
	uint capsule_indices_id = 0;
	float3 vertex_capsule[28];
	uint indices_capsule[145];

	float radius_capsule = 1.0F;
	float latitude_capsule = 1.0F;
};

#endif
