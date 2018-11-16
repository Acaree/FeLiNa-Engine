#include "Importer.h"
#include "Application.h"
#include "ModuleFileSystem.h"

uint Importer::GetLastModificationTime(const char* file)
{
	uint ret = 0;

	if (file != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(file, &buffer);

		if (size > 0)
		{
			JSON_Value* root_value = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root_value);

			ret = json_object_get_number(root_object, "Time");

			json_value_free(root_value);
		}

		RELEASE_ARRAY(buffer);
	}


	return ret;
}