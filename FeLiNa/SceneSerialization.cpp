#include "SceneSerialization.h"
#include "Application.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

SceneSerialization::SceneSerialization()
{
}

SceneSerialization::~SceneSerialization()
{
	for (uint i = 0; i < aux_go.size(); ++i)
	{
		aux_go[i]->CleanUp();
		RELEASE(aux_go[i]);
	}
	aux_go.clear();

	RELEASE(save_name_scene);
	save_name_scene = nullptr;
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

	strcat(save_name_scene, ".json");

	json_serialize_to_file(file_root, save_name_scene);
	json_free_serialized_string(serialized_string);
	json_value_free(file_root);

}

void SceneSerialization::LoadScene(char* file_name)
{
	//TO REVISE and change
	char* file = file_name;
	strcat(file,".json");

	JSON_Value* file_root = json_parse_file(file);

	if (file_root != nullptr)
	{
		JSON_Array* go_array = json_value_get_array(file_root);

		if (go_array != nullptr)
		{
			std::vector<GameObject*> go_vector;
			go_vector.reserve(json_array_get_count(go_array));

			
			for (uint i = 0; i < json_array_get_count(go_array); ++i)
			{
				JSON_Object* object = json_array_get_object(go_array, i);

				GameObject* go = new GameObject(nullptr);
				go->OnLoad(object);

				go_vector.push_back(go);

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


			CreateGameObjectHierarchy(go_vector);

		}


	}
	//json_value_free(file_root);

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

void SceneSerialization::CreateGameObjectHierarchy(std::vector<GameObject*>& aux_go)
{
	/*Future posible bug: If object don't load with parent good or don't save parent this not work*/

	if (aux_go.size() > 0)
	{
		uint uid_root = aux_go[0]->uid;

		for (uint i = 1; i < aux_go.size(); ++i)
		{
			if (aux_go[i]->GetParent() != nullptr)
			{
				if (aux_go[i]->GetParent()->uid == uid_root)
					App->scene->root_object->AddChildren(aux_go[i]);
				else
				{
					GameObject* parent_object = nullptr;
					parent_object = App->scene->root_object->SearchParentForUID(aux_go[i]->GetParent()->uid);

					if (parent_object != nullptr)
						parent_object->AddChildren(aux_go[i]);
					else
						App->scene->root_object->AddChildren(aux_go[i]); // :/

				}
			}

		}
	}

}

void SceneSerialization::ClearActualScene()
{
	App->scene->root_object->CleanData();
	aux_go.clear();
	App->scene->SetSelectedGameObject(nullptr);
	
}