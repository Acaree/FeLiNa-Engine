#ifndef _MODULE_FBX_
#define _MODULE_FBX_

#include "Module.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")

struct ModelData
{
	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;
};

class ModuleFBX : public Module
{
public:
	ModuleFBX(Application* app, bool start_enabled = true);
	~ModuleFBX();
	bool Start();
	bool CleanUp();

	void LoadFbx(const char* path);

public:

	ModelData data;

private:
	aiLogStream stream;
};






#endif