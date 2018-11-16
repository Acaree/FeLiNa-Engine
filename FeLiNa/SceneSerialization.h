#ifndef _SCENE_SERIALIZATION_
#define _SCENE_SERIALIZATION_

#include "Globals.h"
#include "Parson/parson.h"
#include "GameObject.h"
#include <list>

enum FILE_TYPE;

class SceneSerialization
{
public:
	SceneSerialization();
	~SceneSerialization();

	void SaveScene(GameObject* go);

	bool LoadScene(char* file_name = "Assets/Autosave");

	void RecursiveSearchChildrens(GameObject* parent);

	void CreateGameObjectHierarchy(std::vector<GameObject*>& aux_go);
	void GetAllUIDMeshesInMeta(std::list<uint>& uids, const char* file, FILE_TYPE type);
	void GetAllUIDInSerialization(std::list<uint>& uids, const char* file, FILE_TYPE type);

	void ClearActualScene();
private:
	std::vector<GameObject*> aux_go;

public:
	char* save_name_scene = "Auto_Save";
};


#endif