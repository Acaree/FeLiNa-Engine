#include "ModuleTexture.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )

ModuleTexture::ModuleTexture(Application* app,bool start_enabled): Module(app,start_enabled)
{
}

ModuleTexture::~ModuleTexture()
{
}


bool ModuleTexture::LoadTexture(const char* path) const 
{
	uint imageID = 0;				

	uint textureID = 0;			

	bool success = false;			

	ILenum error;				

	ilGenImages(1, &imageID); 		

	ilBindImage(imageID); 			

	success = ilLoadImage(path); 	

	
	if (success)
	{

		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	
		if (!success)
		{
			error = ilGetError();
			LOG( "Image conversion failed - IL reports error: %i %s ", error, iluErrorString(error));
			exit(-1);
		}

		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
			0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

		App->renderer3D->AddTextureData(textureID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		if (success)
			LOG("Texture creation successful.");
		
	}
	else
		LOG("Texture creation failed.");

	ilDeleteImages(1, &imageID); 

	return success;
}