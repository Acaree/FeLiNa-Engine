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

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

private:
	
	Mesh* mesh = nullptr;

};


#endif
