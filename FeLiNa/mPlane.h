#ifndef _MPLANE_
#define _MPLANE_

#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"


class mPlane
{
public:

	mPlane();
	mPlane(float x, float y, float z, float d);

	void			Render() const;
	void			InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			Scale(float x, float y, float z);


public:

	Color color;
	float4x4 transform;
	bool axis;

	bool is_grid = false;
	Plane* matGeo_plane;

};


#endif