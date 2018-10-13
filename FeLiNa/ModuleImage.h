#ifndef _MODULE_IMAGE_
#define _MODULE_IMAGE_

#include <Windows.h>


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

	BYTE * pixels, *pixel_full;
	int width;
	int height;
	char* filename;
	bool start_gif = false;
	Gif_State state = Gif_State::GIF_START;
	
};







#endif
