#ifndef _MATERIAL_IMPORTER_
#define _MATERIAL_IMPORTER_

#include "Importer.h"
#include "ComponentTexture.h"

class Resource;

class MaterialImporter 
{
public:

	MaterialImporter();
	~MaterialImporter();

	//¿LOAD CHECKERS?

	Texture* Import(const char* file_path, std::string& output_file);
	Texture* Import(const void* buffer, uint size, std::string& output_file);

	Texture* LoadDDS(char* path);

	void CreateFileMeta(Resource* resource);
	//bool Load(const char* file_name, Texture* output_texture);
	//bool Load(const void* buffer, uint size, Texture* output_texture);


};





#endif
