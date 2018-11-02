#ifndef _SCENE_SERIALIZATION_
#define _SCENE_SERIALIZATION_

#include "Globals.h"
#include "Parson/parson.h"
#include "GameObject.h"

class SceneSerialization
{
public:
	SceneSerialization();
	~SceneSerialization();

	void SaveScene();

	void LoadScene();

	void RecursiveSearchChildrens(GameObject* parent);

private:
	std::vector<GameObject*> aux_go;

};


#endif