#ifndef _MATERIAL_IMPORTER_
#define _MATERIAL_IMPORTER_

#include "Importer.h"
#include "ComponentTexture.h"
#include <list>

class Resource;
class ResourceMaterial;

struct MaterialSettings : public ImporterSettings //We use this for load/save .meta
{
	// We use the same hexadecimal direction than GL and devil
	enum DXCT_DEFINITION { DXTC_FORMAT = 0, DXT1  , DXT2 ,DXT3 ,DXT4 ,DXT5 };
	DXCT_DEFINITION dxct_compression = DXT1;

	enum TextureWrapMode { CLAMP , REPEAT };
	TextureWrapMode wrap_mode_s = CLAMP;
	TextureWrapMode wrap_mode_t = CLAMP;

	// TO CHANGE : need min_filter

	enum TextureMagFilter {NEAREST,LINEAR };
	TextureMagFilter mag_filter = NEAREST;

};

class MaterialImporter 
{
public:

	MaterialImporter();
	~MaterialImporter();

	//¿LOAD CHECKERS?

	bool Import(const char* file_path, std::string& output_file, const MaterialSettings* material);
	bool Import(const void* buffer, uint size, std::string& output_file, const MaterialSettings* material);


	void CreateFileMeta(std::list<Resource*> resources,  MaterialSettings* settings);
	void ReadFileMeta(const char* file,  MaterialSettings* settings);
	uint GetUIDofMeta(const char* meta);
	void GetImportSettingsInMeta(const char* meta, MaterialSettings* material);

	void SetMaterialSettingsFromMeta(const char* file, MaterialSettings* settings);

	bool Load(const char* file_name, ResourceMaterial* output_texture, const MaterialSettings* settings);
	bool Load(const void* buffer, uint size, ResourceMaterial* output_texture, const MaterialSettings* material_setting);

	void ShowMaterialImport();
public:
	MaterialSettings* material_settings;

};





#endif
