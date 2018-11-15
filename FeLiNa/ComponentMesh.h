#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "Globals.h"
#include "Component.h"


class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	void DrawInspector();

	void SetUID(uint mesh);
	uint GetUID() const;


	void OnSave(JSON_Object* obj); 
	void OnLoad(JSON_Object* obj);

private:
	uint uid = 0;
};


#endif
