#ifndef _COMPONENTMATERIAL
#define _COMPONENTMATERIAL

#include "Component.h"
#include "ComponentMesh.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Globals.h"


class ComponentMaterial : public Component
{
public:
	
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void SetUID(uint uid);
	uint GetUID() const;

	void DrawInspector();

	void OnSave(JSON_Object* obj);
	void OnLoad(JSON_Object* obj);

private:
	uint uid = 0;
};

#endif
