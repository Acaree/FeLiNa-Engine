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


	void apply_texture_to_mesh();


private:

	uint texture_id;
	uint widht;
	uint height;


};

#endif
