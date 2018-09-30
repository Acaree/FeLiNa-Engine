#include "ModuleImage.h"
#include "Application.h"
#include "ModuleWindow.h"

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

