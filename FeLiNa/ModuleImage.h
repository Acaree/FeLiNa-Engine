#ifndef _MODULE_IMAGE_
#define _MODULE_IMAGE_


#pragma comment (lib, "FreeImage/FreeImage.lib")
#include "OpenGL.h"
#include "FreeImage/FreeImage.h"


#include <time.h>
#include <corecrt_wtime.h>

//test
#include "MathGeoLib/Math/float2.h"

#include "SDL/include/SDL_scancode.h"
//-----------



enum Gif_State
{
	GIF_START,
	GIF_RUNING
};

class ModuleImage
{
public:
	ModuleImage(int width, int height);
	~ModuleImage();

	bool TakeScreenshoot();

	void TakeScreenGif(float dt);

private:
	BYTE * pixels;
	int width;
	int height;
	char* filename;

	int FullScreenKey = SDL_Scancode::SDL_SCANCODE_Q;
	bool start_gif = false;
	Gif_State state = Gif_State::GIF_START;
	
};







#endif
