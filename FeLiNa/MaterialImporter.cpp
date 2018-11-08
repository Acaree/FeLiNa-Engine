#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")
#include "mmgr/mmgr.h"

MaterialImporter::MaterialImporter()
{
	// Check versions
	bool same_version = true;

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
		same_version = false;

	if (same_version)
	{
		ilInit();
		iluInit();
		ilutInit();
		ilutRenderer(ILUT_OPENGL);
	}
}


MaterialImporter::~MaterialImporter()
{

}
//const char* importFileName, const char* importPath, std::string& outputFileName
Texture* MaterialImporter::Import(const char* file_name, const char* file_path, std::string& output_file)
{
	Texture* ret = nullptr;

	char* buffer;

	//Check if file is !null
	if (file_path == nullptr) {
		LOG("MATERIAL IMPORTER: Texture imported path is not correct");
		return ret;
	}

	char importer_path[DEFAULT_BUF_SIZE]; //importFilePath

	strcpy_s(importer_path, strlen(file_path) + 1, file_path);

	if (file_name != nullptr)
		strcat_s(importer_path, strlen(importer_path) + strlen(file_name) + 1, file_name);

	output_file = App->fs->GetNameFile(importer_path);

	uint size = App->fs->Load(importer_path, &buffer);

	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Texture load succesful ");

		ret = Import(buffer, size, output_file);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	return ret;
}


Texture* MaterialImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	Texture* ret = new Texture;  //this must be desalocated outside here

	if (buffer == nullptr || size <= 0)
	{
		LOG("BUFFER IS NULL OR SIZE <= 0");
		return ret;
	}

	// Generate the image name
	uint image_id = 0;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);


	// Load the image
	if (ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		ilEnable(IL_FILE_OVERWRITE);

		uint size = 0;
		ILubyte* data = nullptr;

		uint textureID = 0;

		// Pick a specific DXT compression use
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

		// Get the size of the data buffer
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			//TO REVISION REALLY NEED 2?
			ilEnable(IL_FILE_OVERWRITE);

			// Allocate the data buffer
			data = new ILubyte[size];

			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);

			if (ImageInfo.Origin != IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			bool success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

			if (!success)
			{
				LOG("Image conversion failed");
			}

			glGenTextures(1, &textureID);

			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());


			// Save to the buffer
			if (ilSaveL(IL_DDS, data, size) > 0)
			{

				ret->texture_id = textureID;
				ret->width = ilGetInteger(IL_IMAGE_WIDTH);
				ret->height = ilGetInteger(IL_IMAGE_HEIGHT);

				ret->felina_path = new char[DEFAULT_BUF_SIZE]; //this must be desalocated outside here
				char* tmp = App->fs->SaveFile((char*)data, size, output_file, MATERIAL_FILE);
				//TO REVISE TO BAD

				memcpy(ret->felina_path, tmp, DEFAULT_BUF_SIZE);


			}

			RELEASE_ARRAY(data);
		}

		ilDeleteImages(1, &image_id);
	}
	else
		LOG("MATERIAL IMPORTER: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	return ret;
}

Texture* MaterialImporter::LoadDDS(char* path) {

	Texture* ret = new Texture; //this must be desalocated outside here

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
	//	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	//	{
		//	iluFlipImage();
	//	}

		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL reports error: %i %s ", error, iluErrorString(error));
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

		ret->felina_path = path;

		ret->height = ilGetInteger(IL_IMAGE_HEIGHT);
		ret->width = ilGetInteger(IL_IMAGE_WIDTH);
		ret->texture_id = textureID;

		if (success)
			LOG("Texture creation successful.");
	}
	else
		LOG("Texture creation failed.");

	ilDeleteImages(1, &imageID);

	return ret;

}