#include "SceneSerialization.h"
#include "Application.h"
#include "ModuleScene.h"



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



	//Root_Object ¿Need save this?
	aux_go.push_back(App->scene->root_object);
	RecursiveSearchChildrens(App->scene->root_object);

	for (uint i = 0; i < aux_go.size(); ++i)
	{
		JSON_Value* new_value = json_value_init_object();
		JSON_Object* new_object = json_value_get_object(new_value);

		aux_go[i]->OnSave(new_object);

		json_array_append_value(go_array, new_value);

	}
	

	char *serialized_string = json_serialize_to_string_pretty(file_root);
	puts(serialized_string);
	json_serialize_to_file(file_root, "save_test.json");
	json_free_serialized_string(serialized_string);
	json_value_free(file_root);

}

void SceneSerialization::LoadScene()
{
	JSON_Value* file_root = json_parse_file("save_test.json");

	if (file_root != nullptr)
	{
		JSON_Array* go_array = json_value_get_array(file_root);

		if (go_array != nullptr)
		{
			std::vector<GameObject*> go_vector;
			go_vector.reserve(json_array_get_count(go_array));

			

			for (uint i = 0; i <= json_array_get_count(go_array); ++i)
			{
				JSON_Object* object = json_array_get_object(go_array, i);

				GameObject* go = new GameObject(nullptr);
				go->OnLoad(object);

				go_vector.push_back(go);

				//NEED FUNCTION SEARCH CHILDS

			}

			for (uint i = 0; i < json_array_get_count(go_array); ++i)
			{
				JSON_Object* object = json_array_get_object(go_array, i);

				uint parent_uid = json_object_get_number(object, "uid parent");

				for (uint j = 0; j < go_vector.size(); ++j)
				{
		
					if (go_vector[j]->uid == parent_uid)
					{
						go_vector[i]->SetParent(go_vector[j]);
					}
						


				}
			}


			int x = 0;

		}


	}

}

void SceneSerialization::RecursiveSearchChildrens(GameObject* parent)
{

	for (uint i = 0; i < parent->GetNumChildren(); ++i)
	{
		aux_go.push_back(parent->GetChild(i));

		if (parent->GetChild(i)->GetNumChildren() > 0)
		{
			RecursiveSearchChildrens(parent->GetChild(i));
		}
	}


}