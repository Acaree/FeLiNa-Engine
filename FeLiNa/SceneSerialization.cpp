#include "SceneSerialization.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

SceneSerialization::SceneSerialization()
{
	save_name_scene= new char[DEFAULT_BUF_SIZE];
}

SceneSerialization::~SceneSerialization()
{
	RELEASE_ARRAY(save_name_scene);

	aux_go.clear();



}


void SceneSerialization::GetAllUIDMeshesInMeta(std::list<uint>& uids, const char* file, FILE_TYPE type)
{
	//Import All uid that stores in .json (scenes) files for create empty resources
	if (file != nullptr)
	{
		char* buffer;
		std::string tmp = file;

		if(tmp.find(".meta") == std::string::npos)
			tmp += ".meta";
		
		uint size = App->fs->Load(tmp.c_str(), &buffer);

		if (size > 0)
		{
			JSON_Value* root_value = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root_value);

			JSON_Array* array_objects;
			switch (type)
			{
			case MESH_FILE:
				array_objects = json_object_get_array(root_object, "UID MESHES");
				break;
			case MATERIAL_FILE:
				array_objects = json_object_get_array(root_object, "UID MATERIALS");
				break;
			}

			for (uint i = 0; i < json_array_get_count(array_objects); ++i)
				uids.push_back(json_array_get_number(array_objects,i));

			json_value_free(root_value);
		}

		RELEASE_ARRAY(buffer);
	}
}


void SceneSerialization::SaveScene(GameObject* go)
{
	aux_go.clear();

	JSON_Value* file_root = json_value_init_array();
	JSON_Array* go_array = json_value_get_array(file_root);

	//Root_Object ¿Need save this?


	
	RecursiveSearchChildrens(go);

	for (uint i = 0; i < aux_go.size(); ++i)
	{
		JSON_Value* new_value = json_value_init_object();
		JSON_Object* new_object = json_value_get_object(new_value);

		aux_go[i]->OnSave(new_object);

		json_array_append_value(go_array, new_value);

	}
	

	char *serialized_string = json_serialize_to_string_pretty(file_root);
	puts(serialized_string);
	
	//TO REVISION: CAN CRash if use strcpy with same_name_scene
	char name[DEFAULT_BUF_SIZE];

	sprintf_s(name, DEFAULT_BUF_SIZE, "Assets/%s", save_name_scene );
	strcat(name, ".json");
	

	json_serialize_to_file(file_root, name);
	json_free_serialized_string(serialized_string);
	json_value_free(file_root);

	save_name_scene = "";
}

bool SceneSerialization::LoadScene(char* file_name)
{
	//TO REVISE and change
	bool ret = false;

	char file[DEFAULT_BUF_SIZE];

	strcpy_s(file, DEFAULT_BUF_SIZE, file_name);
	strcat(file, ".json");

	JSON_Value* file_root = json_parse_file(file);

	if (file_root != nullptr)
	{
		JSON_Array* go_array = json_value_get_array(file_root);
		ret = true;
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

				if (go->camera != nullptr)
				{
					App->scene->game_camera = go->camera;
				}

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
	save_name_scene = "";
	return ret;
}

void SceneSerialization::RecursiveSearchChildrens(GameObject* parent)
{


	if (parent != App->scene->root_object) {

		aux_go.push_back(parent);
	}

	for (uint i = 0; i < parent->GetNumChildren(); ++i)
	{

		if (parent->GetChild(i)->GetNumChildren() > 0)
		{
			RecursiveSearchChildrens(parent->GetChild(i));
		}
		else
		{
			aux_go.push_back(parent->GetChild(i));
		}
	}


}

void SceneSerialization::CreateGameObjectHierarchy(std::vector<GameObject*>& aux_go)
{
	/*Future posible bug: If object don't load with parent good or don't save parent this not work*/

	if (aux_go.size() > 0)
	{

		for (uint i = 0; i < aux_go.size(); ++i)
		{
			if (aux_go[i]->GetParent() != nullptr)
			{
				
				GameObject* parent_object = nullptr;
				parent_object = App->scene->root_object->SearchParentForUID(aux_go[i]->GetParent()->uid);

				if (parent_object != nullptr)
					parent_object->AddChildren(aux_go[i]);
				else
					App->scene->root_object->AddChildren(aux_go[i]); // :/

				
			}
			else
				App->scene->root_object->AddChildren(aux_go[i]);

		}
	}

}

void SceneSerialization::ClearActualScene()
{
	
	aux_go.clear();
	App->scene->SetSelectedGameObject(nullptr);
}

void SceneSerialization::GetAllUIDInSerialization(std::list<uint>& uids, const char* file, FILE_TYPE type)
{
	char* buffer;
	uint size = App->fs->Load(file, &buffer);

	if (size > 0)
	{
		JSON_Value* root_value = json_parse_string(buffer);
		JSON_Array* array_go = json_value_get_array(root_value);

		for (uint i = 0; i < json_array_get_count(array_go); ++i)
		{
			JSON_Object* object_go = json_array_get_object(array_go, i);
			JSON_Array* components = json_object_get_array(object_go, "Components");

			for (uint i = 0; i < json_array_get_count(components); ++i)
			{
				JSON_Object* component_object = json_array_get_object(components, i);

				switch (type)
				{
				case MESH_FILE:
				{
					int component_type = (int)json_object_get_number(component_object, "type");

					if (component_type == ComponentType::Component_Mesh)
						uids.push_back(json_object_get_number(component_object, "UID"));

					break;
				}
				case MATERIAL_FILE:
				{
					int component_type = json_object_get_number(component_object, "type");

					if (component_type == ComponentType::Component_Material)
						uids.push_back(json_object_get_number(component_object, "UID"));


					break;
				}
				}

			}

		}

		json_value_free(root_value);
	}

	RELEASE_ARRAY(buffer);
}