#include "SceneSerialization.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"


SceneSerialization::SceneSerialization()
{
}

SceneSerialization::~SceneSerialization()
{
}

void SceneSerialization::SaveScene()
{
	JSON_Value* file_root = json_value_init_array();
	JSON_Array* go_array = json_value_get_array(file_root);

	JSON_Value* new_value = json_value_init_object();
	JSON_Object* new_object = json_value_get_object(new_value);

	//Root_Object ¿Need save this?
	App->scene->root_object->OnSave(new_object);
	json_array_append_value(go_array, new_value);

	/*for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		new_value = json_value_init_object();
		new_object = json_value_get_object(new_value);

		App->scene->root_object->GetChild(i)->OnSave(new_object);

		
	}*/
	json_array_append_value(go_array, new_value);

	char *serialized_string = json_serialize_to_string_pretty(file_root);
	puts(serialized_string);
	json_serialize_to_file(file_root, "save_test.json");
	json_free_serialized_string(serialized_string);
	json_value_free(file_root);

}