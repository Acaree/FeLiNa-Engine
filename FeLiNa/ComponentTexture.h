#ifndef _COMPONENTTEXTURE
#define _COMPONENTTEXTURE

#include "Component.h"
#include "ComponentMesh.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Globals.h"

struct Texture;

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* parent);
	~ComponentTexture();
	
	void SetTexture(Texture* texture);
	Texture* GetTexture() const;
	uint GetTextureID() const;

private:

	Texture* texture = nullptr;
};

#endif
