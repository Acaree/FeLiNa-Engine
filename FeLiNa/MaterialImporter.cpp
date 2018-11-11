#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleTimeManagement.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ModuleGui.h"
#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"
#include "ImGui/imgui.h"
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

	material_settings = new MaterialSettings();
}


MaterialImporter::~MaterialImporter()
{
	RELEASE(material_settings);
}


Texture* MaterialImporter::Import(const char* file_path, std::string& output_file)
{
	Texture* ret = nullptr;

	char* buffer;

	//Check if file is !null
	if (file_path == nullptr) {
		LOG("MATERIAL IMPORTER: Texture imported path is not correct");
		return ret;
	}

	else {

		std::string file_name = file_path;
		file_name = file_name.substr(file_name.find_last_of("//") + 1, file_name.size());
		output_file = file_name.substr(0, file_name.find_last_of("."));

	}
	
	uint size = App->fs->Load(file_path, &buffer);

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
	Texture* ret = new Texture();  //this must be desalocated outside here

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

				RELEASE_ARRAY(tmp);
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

	Texture* ret = new Texture(); //this must be desalocated outside here

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

void MaterialImporter::CreateFileMeta(Resource* resource, MaterialSettings* settings)
{
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);

	json_object_set_number(root_object, "Time", App->time_management->ReadRealTimeClock());
	json_object_set_number(root_object, "UID", resource->GetUID());

	JSON_Value* material_import = json_value_init_object();
	JSON_Object* settings_import = json_value_get_object(material_import);

	json_object_set_value(root_object, "Import Settings", material_import);

	json_object_set_boolean(settings_import, "DXCT Compression",settings->dxct_compression); //If we save as number , when we rean can convet the type :/
	json_object_set_boolean(settings_import, "WRAP Mode S", settings->wrap_mode_s);
	json_object_set_boolean(settings_import, "WRAP Mode T", settings->wrap_mode_t);
	json_object_set_boolean(settings_import, "MAG FILTER", settings->mag_filter);
	

	char path[DEFAULT_BUF_SIZE];
	strcpy(path, resource->GetExportedFile());
	strcat(path, ".meta");

	uint size = json_serialization_size_pretty(root_value);
	char* buffer = new char[size];

	json_serialize_to_buffer_pretty(root_value, buffer, size);

	App->fs->SaveBufferData(buffer,path,size); 

	RELEASE_ARRAY(buffer);

}

void MaterialImporter::ReadFileMeta(const char* file, MaterialSettings* settings)
{
	char* buffer = nullptr;

	uint size = App->fs->Load(file, &buffer);

	if (size > 0)
	{
		LOG("Can load Mesh meta file: %s", file);

		JSON_Value* root = json_parse_string(buffer);
		JSON_Object* root_object = json_value_get_object(root);

		JSON_Object* import_settings = json_object_get_object(root_object, "Import Settings");

		settings->dxct_compression = (MaterialSettings::DXCT_DEFINITION)json_object_get_boolean(import_settings, "DXCT Compression");
		settings->wrap_mode_s = (MaterialSettings::TextureWrapMode)json_object_get_boolean(import_settings, "WRAP Mode S");
		settings->wrap_mode_t = (MaterialSettings::TextureWrapMode)json_object_get_boolean(import_settings, "WRAP Mode T");
		settings->mag_filter = (MaterialSettings::TextureMagFilter)json_object_get_boolean(import_settings, "MAG FILTER");

		json_value_free(root);
	}
	else
		LOG("Error reading mesh meta file: %s", file);

	RELEASE_ARRAY(buffer);
}

void MaterialImporter::ShowMaterialImport()
{

	static int current_compression = material_settings->dxct_compression;
	const char* compression_elements[] = {"DXTC_FORMAT" , "DXT1" , "DXT2" ,"DXT3" ,"DXT4" ,"DXT5"  };

	if (ImGui::Combo("###compression2", &current_compression, compression_elements, ((int)(sizeof(compression_elements) / sizeof(*compression_elements)))))
	{
		material_settings->dxct_compression = (MaterialSettings::DXCT_DEFINITION)current_compression;
	}

	static int current_wrap_s = material_settings->wrap_mode_s;
	static int current_wrap_t = material_settings->wrap_mode_t;
	const char* wrap_elements[] = { "CLAMP" , "REPEAT"  };

	if (ImGui::Combo("###wrap_s", &current_wrap_s, wrap_elements, ((int)(sizeof(wrap_elements) / sizeof(*wrap_elements)))))
	{
		material_settings->wrap_mode_s = (MaterialSettings::TextureWrapMode)current_wrap_s;
	}

	if (ImGui::Combo("###wrap_t", &current_wrap_t, wrap_elements, ((int)(sizeof(wrap_elements) / sizeof(*wrap_elements)))))
	{
		material_settings->wrap_mode_t = (MaterialSettings::TextureWrapMode)current_wrap_t;
	}

	static int current_mag = material_settings->mag_filter;
	const char* mag_elements[] = { "NEAREST" ,"LINEAR" };

	if (ImGui::Combo("###mag_filter", &current_mag, mag_elements, ((int)(sizeof(mag_elements) / sizeof(*mag_elements)))))
	{
		material_settings->mag_filter = (MaterialSettings::TextureMagFilter)current_mag;
	}
	
	if (ImGui::Button("IMPORT ###importmaterial", { 70,50 }))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel ###savematerial", { 70,50 }))
	{
		App->gui->file_focus.clear();
	}

}