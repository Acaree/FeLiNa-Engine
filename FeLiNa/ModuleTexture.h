#ifndef _MODULE_TEXTURE_
#define _MODULE_TEXTURE_

#include "Module.h"

class ComponentTexture;

class ModuleTexture : public Module
{
public:
	ModuleTexture(Application* app,bool start_enabled = true);
	~ModuleTexture();

	bool Init();
	ComponentTexture* LoadTexture(const char* path, int index) const;

};


#endif
