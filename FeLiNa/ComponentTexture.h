#ifndef _COMPONENTTEXTURE
#define _COMPONENTTEXTURE

#include "Component.h"
#include "ComponentMesh.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Globals.h"

struct Texture
{
	uint texture_id = 0;
	uint width = 0;
	uint height = 0;

};

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* parent);
	~ComponentTexture();
	
	void SetTexture(Texture* texture);
	Texture* GetTexture() const;
	uint GetTextureID() const;
	uint GetTextureHeight() const;
	uint GetTextureWidth() const;

	void DrawInspector();

	//void OnSave(JSON_Object* obj); //What we save? :/
	//void OnLoad(JSON_Object* obj);

	bool material_checker = false;
	bool no_texture = false;

private:

	Texture* texture = nullptr;

};

#endif
