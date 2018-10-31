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

bool ModuleTexture::LoadTexture(const char* path,int index)
{

	uint imageID = 0;				

	uint textureID = 0;			

	bool success = false;			

	ILenum error;				

	ilGenImages(1, &imageID); 		

	ilBindImage(imageID);

	std::string path_s = ("%s", path);

	success = ilLoadImage(path); 	


	if (success)
	{
		Texture* texture = new Texture();

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
			LOG("Image conversion failed - IL reports error: %i %s ", error, iluErrorString(error));

		}

		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
			0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());


		texture->texture_id = textureID;
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);

		textures.push_back(texture);


		if (success) {
			LOG("Texture creation successful.");
			SaveTextureAsDDS(path_s, &textureID);
		}

	}
	else
		LOG("Texture creation failed.");

	ilDeleteImages(1, &imageID); 

	

	return success;
}

void ModuleTexture::AddTexture(Texture* tex)
{
	textures.push_back(tex);
}

ComponentTexture* ModuleTexture::CreateComponentTexture()
{
	ComponentTexture* c_texture = new ComponentTexture(nullptr);
	c_texture->SetTexture(textures[textures.size() - 1]);

	return c_texture;
}

bool ModuleTexture::SaveTextureAsDDS(std::string& output_path, void* data) {
	
	bool ret = false;
	
	ILuint   size; 
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use 
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer 
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer    
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function        
			ret = App->fs->SaveFile(output_path, data, size, "Phys_saves", "texture", "dds"); //first folder path, then filename and last extension
			RELEASE_ARRAY(data);
	}

	return ret;
}