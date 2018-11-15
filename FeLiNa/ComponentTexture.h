#ifndef _COMPONENTTEXTURE
#define _COMPONENTTEXTURE

#include "Component.h"
#include "ComponentMesh.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Globals.h"


class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* parent);
	~ComponentTexture();

	void SetUID(uint uid);
	uint GetUID() const;

	void DrawInspector();

	void OnSave(JSON_Object* obj); //What we save? :/
	void OnLoad(JSON_Object* obj);

private:
	uint uid = 0;
};

#endif
