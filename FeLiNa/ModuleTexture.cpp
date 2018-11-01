#include "ModuleTexture.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ComponentTexture.h"
#include "ModuleFileSystem.h"
#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
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

bool ModuleTexture::Init()
{
	bool ret = true;

	// Check versions
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG(" ERROR: DevIL version is different.");
		ret = false;
	}
	else
	{
		ilutRenderer(ILUT_OPENGL);
		ilInit();
		iluInit();
		ilutInit();
		ilutRenderer(ILUT_OPENGL);
	}

	return ret;
}

bool ModuleTexture::ImportTexture(std::string path, const void* buffer, uint size)
{
	bool success = false;
	
/*	if (buffer != nullptr && size > 0)
	{
		ILuint imageID = 0;

		ILenum error;

		ilGenImages(1, &imageID);

		ilBindImage(imageID);

		success = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);


		if (ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
		{

			Texture* texture = new Texture();


			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			

			ILuint   size;
			ILubyte *data = nullptr;
			// To pick a specific DXT compression use 
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
			if (size > 0)
			{
				ilEnable(IL_FILE_OVERWRITE);
				data = new ILubyte[size]; // allocate data buffer
				if (ilSaveL(IL_DDS, data, size) > 0) // Save with the ilSaveIL function				

				texture->texture_id = imageID;
				texture->width = ilGetInteger(IL_IMAGE_WIDTH);
				texture->height = ilGetInteger(IL_IMAGE_HEIGHT);

				textures.push_back(texture);

				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &imageID);
		}
	}*/

	return success;
}

void ModuleTexture::AddTexture(Texture* tex)
{
	textures.push_back(tex);
}

ComponentTexture* ModuleTexture::CreateComponentTexture()
{
	ComponentTexture* c_texture = new ComponentTexture(nullptr);
	//c_texture->SetTexture(textures[textures.size() - 1]);

	return c_texture;
}

