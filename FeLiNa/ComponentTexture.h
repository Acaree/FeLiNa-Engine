#ifndef _COMPONENTTEXTURE
#define _COMPONENTTEXTURE

#include "Component.h"
#include "ComponentMesh.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Globals.h"

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* parent, uint texture_id);
	~ComponentTexture();

	
	void ApplyTextureToMesh();

	uint texture_id = 0;

	uint widht = 0;
	uint height = 0;

private:




};

#endif
