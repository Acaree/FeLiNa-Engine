#include "MaterialImporter.h"
#include "PhysFS/physfs.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTimeManagement.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ResourceMaterial.h"
#include "ModuleGui.h"
#include "ResourceMaterial.h"
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


bool MaterialImporter::Import(const char* file_path, std::string& output_file, const MaterialSettings* material)
{
	bool ret = false;

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

		ret = Import(buffer, size, output_file, material);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	return ret;
}


bool MaterialImporter::Import(const void* buffer, uint size, std::string& output_file, const MaterialSettings* material)
{
	bool ret = false;

	if (buffer == nullptr || size <= 0 || material == nullptr)
		return ret;
		
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

		int compression_format = 0;

		switch (material->dxct_compression)
		{
		case MaterialSettings::DXTC_FORMAT:
			compression_format = IL_DXTC_FORMAT;
			break;
		case MaterialSettings::DXT1:
			compression_format = IL_DXT1;
			break;
		case MaterialSettings::DXT2:
			compression_format = IL_DXT2;
			break;
		case MaterialSettings::DXT3:
			compression_format = IL_DXT3;
			break;
		case MaterialSettings::DXT4:
			compression_format = IL_DXT4;
			break;
		case MaterialSettings::DXT5:
			compression_format = IL_DXT5;
			break;
		}

		// Pick a specific DXT compression use: set in importsettings
		ilSetInteger(IL_DXTC_FORMAT, compression_format);

		// Get the size of the data buffer
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			ilEnable(IL_FILE_OVERWRITE);

			// Allocate the data buffer
			data = new ILubyte[size];

			if (ilSaveL(IL_DDS, data, size) > 0)
			{

				uint uid = App->GenerateRandomNumber();
				output_file = std::to_string(uid);

				App->fs->SaveFile((char*)data, size, output_file, MATERIAL_FILE);

				ret = true;
			}
			else
				LOG("Can't save texture int own format");
			

			RELEASE_ARRAY(data);
		}

		ilDeleteImages(1, &image_id);
	}
	else
		LOG("MATERIAL IMPORTER: DevIL could not load the image. ERROR: %s", iluErrorString(ilGetError()));

	return ret;
}

bool MaterialImporter::Load(const char* file_name, ResourceMaterial* output_texture, const MaterialSettings* settings)
{
	bool ret = false;

	if (file_name == nullptr || output_texture == nullptr || settings == nullptr)
		return ret;

	char* buffer;
	uint size = App->fs->Load(file_name,&buffer);

	if (size > 0)
	{
		LOG("LOAD MATERIAL IMPORT GOOD");
		ret = Load(buffer, size, output_texture, settings);
		RELEASE_ARRAY(buffer);
	}

	return ret;
	
}

bool MaterialImporter::Load(const void* buffer, const uint size, ResourceMaterial* output_texture, const MaterialSettings* material_setting)
{
	bool ret = false;

	if (buffer == nullptr || output_texture == nullptr || material_setting == nullptr)
		return ret;

	uint image_name = 0;
	ilGenImages(1, &image_name);
	ilBindImage(image_name);

	if (ilLoadL(IL_DDS, buffer, size))
	{
		ILinfo imageInfo;
		iluGetImageInfo(&imageInfo);

		if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			uint tex_id = 0;

			glGenTextures(1, &tex_id);
			glBindTexture(GL_TEXTURE_2D, tex_id);


			int wrap_mode_s = 0;
			int wrap_mode_t = 0;
			int mag_filter = 0;

			switch (material_setting->wrap_mode_s)
			{
			case MaterialSettings::TextureWrapMode::CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				break;
			case MaterialSettings::TextureWrapMode::REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				break;
			default:
				break;
			}

			switch (material_setting->wrap_mode_t)
			{
			case MaterialSettings::TextureWrapMode::CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				break;
			case MaterialSettings::TextureWrapMode::REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			default:
				break;
			}

			switch (material_setting->mag_filter)
			{
			case MaterialSettings::TextureMagFilter::NEAREST:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case MaterialSettings::TextureMagFilter::LINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			default:
				break;
			}



			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

			output_texture->id_texture = tex_id;
			output_texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
			output_texture->widht = ilGetInteger(IL_IMAGE_WIDTH);

			glBindTexture(GL_TEXTURE_2D, 0);

			ret = true;
		}

		ilDeleteImages(1, &image_name);
	}

	return ret;
}


void MaterialImporter::CreateFileMeta(std::list<Resource*> resources, MaterialSettings* settings)
{

	if (resources.size() > 0 && settings != nullptr)
	{
		ResourceMaterial* res_material = (ResourceMaterial*)resources.front();

		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);

		ResourceMaterial* resource = (ResourceMaterial*)resources.front();
		json_object_set_number(root_object, "Time", App->fs->GetLastModificationTime(resource->exported_file.c_str()));

		JSON_Value* array_value = json_value_init_array();
		JSON_Array* array_uids = json_value_get_array(array_value);

		for(std::list<Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
			json_array_append_number(array_uids,  (*it)->GetUID());

		json_object_set_value(root_object, "UID MATERIALS", array_value);

		JSON_Value* material_import = json_value_init_object();
		JSON_Object* settings_import = json_value_get_object(material_import);

		json_object_set_value(root_object, "Import Settings", material_import);

		json_object_set_number(settings_import, "DXCT Compression", settings->dxct_compression);
		json_object_set_boolean(settings_import, "WRAP Mode S", settings->wrap_mode_s);
		json_object_set_boolean(settings_import, "WRAP Mode T", settings->wrap_mode_t);
		json_object_set_boolean(settings_import, "MAG FILTER", settings->mag_filter);


		char path[DEFAULT_BUF_SIZE];
		strcpy(path, res_material->GetExportedFile());
		strcat(path, ".meta");

		uint size = json_serialization_size_pretty(root_value);
		char* buffer = new char[size];

		json_serialize_to_buffer_pretty(root_value, buffer, size);

		App->fs->SaveBufferData(buffer, path, size);


		RELEASE_ARRAY(buffer);
	}
}


void MaterialImporter::ShowMaterialImport()
{

	static int current_compression = material_settings->dxct_compression;
	static const char* compression_elements[] = {"DXTC_FORMAT" , "DXT1" , "DXT2" ,"DXT3" ,"DXT4" ,"DXT5"  };

	if (ImGui::Combo("###compression2", &current_compression, compression_elements, ((int)(sizeof(compression_elements) / sizeof(*compression_elements)))))
	{
		material_settings->dxct_compression = (MaterialSettings::DXCT_DEFINITION)current_compression;
	}

	static int current_wrap_s = material_settings->wrap_mode_s;
	static int current_wrap_t = material_settings->wrap_mode_t;
	static const char* wrap_elements[] = { "CLAMP" , "REPEAT"  };

	if (ImGui::Combo("###wrap_s", &current_wrap_s, wrap_elements, ((int)(sizeof(wrap_elements) / sizeof(*wrap_elements)))))
	{
		material_settings->wrap_mode_s = (MaterialSettings::TextureWrapMode)current_wrap_s;
	}

	if (ImGui::Combo("###wrap_t", &current_wrap_t, wrap_elements, ((int)(sizeof(wrap_elements) / sizeof(*wrap_elements)))))
	{
		material_settings->wrap_mode_t = (MaterialSettings::TextureWrapMode)current_wrap_t;
	}

	static int current_mag = material_settings->mag_filter;
	static const char* mag_elements[] = { "NEAREST" ,"LINEAR" };

	if (ImGui::Combo("###mag_filter", &current_mag, mag_elements, ((int)(sizeof(mag_elements) / sizeof(*mag_elements)))))
	{
		material_settings->mag_filter = (MaterialSettings::TextureMagFilter)current_mag;
	}
	
	if (ImGui::Button("Apply ###importmaterial", { 70,50 }))
	{
		std::string tmp = App->gui->file_focus;
		tmp += ".meta";

		SetMaterialSettingsFromMeta(tmp.c_str(),material_settings);

		App->fs->refresh_now = true;

		App->gui->file_focus.clear();
		App->gui->show_import_settings = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel ###savematerial", { 70,50 }))
	{
		App->gui->show_import_settings = false;
	}

}

void MaterialImporter::SetMaterialSettingsFromMeta(const char* file, MaterialSettings* settings) const
{
	if (file != nullptr && settings != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(file, &buffer);

		if (size > 0)
		{
			LOG("Can load Mesh meta file: %s", file);

			JSON_Value* root = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root);

			json_object_set_number(root_object, "Time", 1);

			JSON_Object* settings_import = json_object_get_object(root_object, "Import Settings");

			json_object_set_number(settings_import, "DXCT Compression", settings->dxct_compression); 
			json_object_set_boolean(settings_import, "WRAP Mode S", settings->wrap_mode_s);
			json_object_set_boolean(settings_import, "WRAP Mode T", settings->wrap_mode_t);
			json_object_set_boolean(settings_import, "MAG FILTER", settings->mag_filter);


			uint size = json_serialization_size_pretty(root);
			char* new_buffer = new char[size];

			json_serialize_to_buffer_pretty(root, new_buffer, size);

			App->fs->SaveBufferData(new_buffer, file, size);

			RELEASE_ARRAY(new_buffer);
			json_value_free(root);
		}
		else
			LOG("Error reading mesh meta file: %s", file);

		RELEASE_ARRAY(buffer);
	}



}


void MaterialImporter::GetImportSettingsInMeta(const char* meta, MaterialSettings* settings)
{
	if (meta != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(meta, &buffer);

		if (size > 0)
		{
			JSON_Value* root_value = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root_value);

			JSON_Object* import_settings = json_object_get_object(root_object, "Import Settings");

			settings->dxct_compression = (MaterialSettings::DXCT_DEFINITION)(int)json_object_get_number(import_settings, "DXCT Compression");
			settings->wrap_mode_s = (MaterialSettings::TextureWrapMode)json_object_get_boolean(import_settings, "WRAP Mode S");
			settings->wrap_mode_t = (MaterialSettings::TextureWrapMode)json_object_get_boolean(import_settings, "WRAP Mode T");
			settings->mag_filter = (MaterialSettings::TextureMagFilter)json_object_get_boolean(import_settings, "MAG FILTER");

			json_value_free(root_value);
		}

		RELEASE_ARRAY(buffer);
		
	}

}

uint MaterialImporter::GetUIDofMeta(const char* meta) const
{
	uint ret = 0;

	if (meta != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(meta, &buffer);

		if (size > 0)
		{
			JSON_Value* root_value = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root_value);

			JSON_Array* array_objects;
			array_objects = json_object_get_array(root_object, "UID MATERIALS");

			ret = json_array_get_number(array_objects, 0);
			

			json_value_free(root_value);
		}

		RELEASE_ARRAY(buffer);

	}

	return ret;
}