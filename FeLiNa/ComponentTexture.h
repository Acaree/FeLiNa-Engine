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

	uint texture_id;

	uint widht;
	uint height;

private:




};

#endif
