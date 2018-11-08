#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include <string>

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

	char* felina_path = nullptr;

};



class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	void CleanUp();

	void DrawInspector();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

	void SetPath(char* path);

	void OnSave(JSON_Object* obj); // Same that Texture :/
	void OnLoad(JSON_Object* obj);

private:
	
	Mesh* mesh = nullptr;

};


#endif
