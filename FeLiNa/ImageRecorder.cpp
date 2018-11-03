#include "ImageRecorder.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Gif/gif.h"
#include "Devil\include\ilut.h"
#include <time.h>

ImageRecorder::ImageRecorder(int widht, int height) : width(widht), height(height)
{
	pixels = new BYTE[3 * widht*height];
	pixel_full = new BYTE[width*height * sizeof(unsigned char)*4];
}

ImageRecorder::~ImageRecorder()
{
	RELEASE_ARRAY(pixels);
	RELEASE_ARRAY(pixel_full);
}

bool ImageRecorder::TakeScreenshoot()
{
	
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilutGLScreen();
	ilEnable(IL_FILE_OVERWRITE);

	time_t actual_time = time(0);
	tm reported_time;
	localtime_s(&reported_time, &actual_time);

	static char tmp[50];
	sprintf_s(tmp, 1024, "Screenshots_gif/Screenshot_%i_%i_%i.%i.%i.gif", reported_time.tm_mday, reported_time.tm_mon+1, reported_time.tm_hour, reported_time.tm_min, reported_time.tm_sec);

	ilSave(IL_PNG, tmp);
	ilDisable(IL_FILE_OVERWRITE);
	ilDeleteImage(imageID);
	
	return true;
}


GifWriter writer;

void ImageRecorder::TakeScreenGif(float dt)
{

	switch (state)
	{
	case GIF_START:

		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			time_t actual_time = time(0);
			tm reported_time;
			localtime_s(&reported_time, &actual_time);

			static char tmp[50];
			sprintf_s(tmp, 1024, "Screenshots_gif/Gif_%i_%i_%i.%i.%i.gif", reported_time.tm_mday, reported_time.tm_mon+1, reported_time.tm_hour, reported_time.tm_min, reported_time.tm_sec);


			bool gif_in_progress = GifBegin(&writer, tmp, width, height, (uint32_t)(dt * 100.0f), 8, false);
			if (gif_in_progress)
			{
				state = Gif_State::GIF_RUNING;
			}
		}
		break;
	case GIF_RUNING:

		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixel_full);

		GifWriteFrame(&writer, pixel_full, width, height, (uint32_t)(dt * 100.0f), 8, false);
		
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			GifEnd(&writer);
			
			state = Gif_State::GIF_START;
		}

		break;

	}
}

bool ImageRecorder::CleanUp() {

	RELEASE_ARRAY(pixels);
	RELEASE_ARRAY(pixel_full);
	RELEASE(filename);

	return true;

}