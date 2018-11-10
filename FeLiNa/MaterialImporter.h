#ifndef _MATERIAL_IMPORTER_
#define _MATERIAL_IMPORTER_

#include "Importer.h"
#include "ComponentTexture.h"

class Resource;


struct MaterialSettings : public ImporterSettings //We use this for load/save .meta
{
	// We use the same hexadecimal direction than GL and devil
	enum DXCT_DEFINITION { DXTC_FORMAT = 0x0705, DXT1 = 0x0706, DXT2 = 0x0707,DXT3 = 0x0708,DXT4 = 0x0709,DXT5 = 0x070A};
	DXCT_DEFINITION dxct_compression = DXT1;

	enum TextureWrapMode { CLAMP = 0x2900, REPEAT = 0x2901};
	TextureWrapMode wrap_mode_s = CLAMP;
	TextureWrapMode wrap_mode_t = CLAMP;

	enum TextureMagFilter {NEAREST = 0x2600,LINEAR = 0x2601};
	TextureMagFilter mag_filter = NEAREST;

};

class MaterialImporter 
{
public:

	MaterialImporter();
	~MaterialImporter();

	//¿LOAD CHECKERS?

	Texture* Import(const char* file_path, std::string& output_file);
	Texture* Import(const void* buffer, uint size, std::string& output_file);

	Texture* LoadDDS(char* path);

	void CreateFileMeta(Resource* resource, MaterialSettings* settings);
	void ReadFileMeta(const char* file, MaterialSettings* settings);
	//bool Load(const char* file_name, Texture* output_texture);
	//bool Load(const void* buffer, uint size, Texture* output_texture);

};





#endif
