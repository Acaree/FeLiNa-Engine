#ifndef _IMPORTER_H
#define _IMPORTER_H

#include "Globals.h"
#include <string>

//We use this for load/save .meta
struct ImporterSettings 
{

};

class Importer
{
public:

	Importer() {}
	virtual ~Importer() {}

	virtual bool Import(const char* file_name, const char* file_path, std::string& output_file) { return false; }
	virtual bool Import(const void* buffer, uint size, std::string& output_file) { return false; }

	uint GetLastModificationTime(const char* file);

};



#endif
