#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "Globals.h"
#include "Component.h"

struct Mesh;


class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	void DrawInspector();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

	//void OnSave(JSON_Object* obj); // Same that Texture :/
	//void OnLoad(JSON_Object* obj);

private:
	
	Mesh* mesh = nullptr;

};


#endif
