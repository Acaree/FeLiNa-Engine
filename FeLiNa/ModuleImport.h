#ifndef _MODULE_IMPORT_
#define _MODULE_IMPORT_

#include "Module.h"

class aiScene;
struct aiLogStream;
struct aiMaterial;

class ModuleImport : public Module
{
public:
	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Start();
	bool CleanUp();
	
	void LoadData(const char* path);
	void FindTexturePath(aiMaterial* material, const char* path, int index );
};






#endif