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
bool MaterialImporter::Import(const char* file_name, const char* file_path, std::string& output_file)
{
	bool ret = false;

	char* buffer;

	//Check if file is !null
	if (file_path == nullptr)
		return ret;

	char importer_path[DEFAULT_BUF_SIZE]; //importFilePath

	strcpy_s(importer_path, strlen(file_path) + 1, file_path); 

	if (file_name != nullptr)
		strcat_s(importer_path, strlen(importer_path) + strlen(file_name) + 1, file_name);

	output_file = App->fs->GetNameFile(importer_path);

	uint size = App->fs->Load(importer_path, &buffer);

	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Successfully");

		ret = Import(buffer, size, output_file);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	return ret;
}

//const void* buffer, uint size, std::string& outputFileName
bool MaterialImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	bool ret = false;

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

			// Save to the buffer
			if (ilSaveL(IL_DDS, data, size) > 0)
			{
				if (App->fs->SaveTexture((char*)data, size, output_file) > 0)
				{
					LOG("MATERIAL IMPORTER: Successfully saved texture %s to own format", output_file);
					ret = true;
				}
				else
					LOG("MATERIAL IMPORTER: Could not save texture %s to own format", output_file);
			}

			RELEASE_ARRAY(data);
		}

		ilDeleteImages(1, &image_id);
	}
	else
		LOG("MATERIAL IMPORTER: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	return ret;
}