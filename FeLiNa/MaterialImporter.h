#ifndef _MATERIAL_IMPORTER_
#define _MATERIAL_IMPORTER_

#include "Importer.h"

struct Texture_Importer
{
	uint id = 0;
	uint width = 0;
	uint height = 0;
};


class MaterialImporter :public Importer
{
public:

	MaterialImporter();
	~MaterialImporter();

	//¿LOAD CHECKERS?

	bool Import(const char* file_name, const char* file_path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	//bool Load(const char* file_name, Texture* output_texture);
	//bool Load(const void* buffer, uint size, Texture* output_texture);


};





#endif
