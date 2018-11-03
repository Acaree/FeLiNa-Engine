#ifndef _IMAGE_RECORDER_
#define _IMAGE_RECORDER_


#include "Globals.h"
#include <Windows.h>


enum Gif_State
{
	GIF_START,
	GIF_RUNING
};

class ImageRecorder
{
public:
	ImageRecorder(int width, int height);
	~ImageRecorder();

	bool CleanUp();

	bool TakeScreenshoot();

	void TakeScreenGif(float dt);

private:

	BYTE * pixels = nullptr;
	BYTE *pixel_full = nullptr;
	int width = 0;
	int height = 0;
	char* filename = nullptr;
	bool start_gif = false;
	Gif_State state = Gif_State::GIF_START;
	
};







#endif
