#ifndef _MODULE_IMAGE_
#define _MODULE_IMAGE_


#pragma comment (lib, "FreeImage/FreeImage.lib")
#include "OpenGL.h"
#include "FreeImage/FreeImage.h"


class ModuleImage
{
public:
	ModuleImage(int width, int height);
	~ModuleImage();

	bool TakeScreenshoot();

private:
	BYTE * pixels;
	int width;
	int height;
	char* filename;
	int number_filename = 0; 
};





#endif
