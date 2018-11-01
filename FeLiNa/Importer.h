#ifndef _IMPORTER_H
#define _IMPORTER_H

#include "Globals.h"
#include <string>



class Importer
{
public:

	Importer() {}
	virtual ~Importer() {}

	virtual bool Import(const char* file_name, const char* file_path, std::string& output_file) { return false; }
	virtual bool Import(const void* buffer, uint size, std::string& output_file) { return false; }

};









#endif
