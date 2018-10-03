#ifndef _MODULE_FBX_
#define _MODULE_FBX_


#include "Module.h"


class ModuleFBX : public Module
{
public:
	ModuleFBX(Application* app, bool start_enabled = true);
	~ModuleFBX();

	bool CleanUp();

	bool LoadFbx(const char* path);
	

};






#endif