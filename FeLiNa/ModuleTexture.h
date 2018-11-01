#ifndef _MODULE_TEXTURE_
#define _MODULE_TEXTURE_

#include "Module.h"

class ComponentTexture;

struct Texture
{
	uint texture_id = 0;
	uint width = 0;
	uint height = 0;

};

class ModuleTexture : public Module
{
public:
	ModuleTexture(Application* app,bool start_enabled = true);
	~ModuleTexture();

	bool Init();

	bool ImportTexture(std::string path, const void* buffer, uint size);

	void AddTexture(Texture* tex);
	
	ComponentTexture* CreateComponentTexture();

private:
	std::vector<Texture*> textures;
};


#endif
