#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "Globals.h"
#include "Component.h"

struct Mesh
{
	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_uv = 0;
	uint num_uv = 0;
	float* uv = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;
};


class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	void Draw();

	void SetMesh(Mesh* mesh);

	//Reading Mesh Data functions?
	
private:
	
	Mesh* mesh;
	//Here AABB?

};


#endif
