#ifndef _MODULE_IMPORT_
#define _MODULE_IMPORT_

#include "Module.h"
#include "OpenGL.h"



class aiScene;
struct aiLogStream;


class ModuleImport : public Module
{
public:
	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();
	bool Start();
	bool CleanUp();

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	GLuint LoadTexture(const char* theFileName);

	void LoadData(const char* path);
	

};






#endif