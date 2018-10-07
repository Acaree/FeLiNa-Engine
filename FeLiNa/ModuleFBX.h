#ifndef _MODULE_FBX_
#define _MODULE_FBX_

#include "Module.h"
#include "OpenGL.h"
#include "Assimp/include/cimport.h"
#pragma comment (lib,"Assimp/libx86/assimp.lib")

class aiScene;
struct aiLogStream;

struct ModelData
{
	GLuint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	GLuint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	GLuint id_texture = 0;
	uint num_texture = 0;
	float* texture = nullptr;

	GLuint id_color = 0;
	uint num_color = 0;
	float* colors = nullptr;
	aiColor4D color_4D;
};

class ModuleFBX : public Module
{
public:
	ModuleFBX(Application* app, bool start_enabled = true);
	~ModuleFBX();
	bool Start();
	bool CleanUp();

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

	void LoadFbx(const char* path);
	
public:

	ModelData data;


};






#endif