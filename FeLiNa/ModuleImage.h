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

	bool CleanUp();

	bool TakeScreenshoot();

	void TakeScreenGif(float dt);

private:

	BYTE * pixels = nullptr;
	BYTE *pixel_full = nullptr;
	int width = 0;
	int height = 0;
	char* filename;
	bool start_gif = false;
	Gif_State state = Gif_State::GIF_START;
	
};







#endif
