#include "ModuleImage.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Gif/gif.h"
ModuleImage::ModuleImage(int widht, int height) : width(widht), height(height)
{
	pixels = new BYTE[3 * widht*height];
}

ModuleImage::~ModuleImage()
{
	delete[]pixels;
}

bool ModuleImage::TakeScreenshoot()
{
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels,width,height,3*width,24, 0x0000FF, 0xFF0000, 0x00FF00, false);
	FreeImage_Save(FIF_PNG, image, "Screenshots/test.png");

	FreeImage_Unload(image);
	return true;
}


GifWriter writer;

void ModuleImage::TakeScreenGif(float dt)
{
	
	switch (state)
	{
	case GIF_START:

		if (App->input->GetKey(FullScreenKey) == KEY_DOWN)
		{
			char tmp[20];
			sprintf_s(tmp, 100, "Screenshots/test.gif");
			
			bool gif_in_progress = GifBegin(&writer, tmp, width, height, (uint32_t)(dt * 100.0f), 8, false);
			if (gif_in_progress)
			{
				state = Gif_State::GIF_RUNING;
			}
		}
		break;
	case GIF_RUNING:

		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
		GifWriteFrame(&writer, pixels, width, height, (uint32_t)(dt * 100.0f), 8, false);

		if (App->input->GetKey(FullScreenKey) == KEY_DOWN)
		{
			GifEnd(&writer);
			
			state = Gif_State::GIF_START;
		}

		break;

	}


	

}