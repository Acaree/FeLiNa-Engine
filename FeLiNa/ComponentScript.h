#ifndef _COMPONENT_SCRIPT_
#define _COMPONENT_SCRIPT_

#include "Globals.h"
#include "Component.h"

class GameObject;

class ComponentScript : public Component
{
public:

	ComponentScript(GameObject* parent);
	~ComponentScript();

	void Update(float dt);
	void DrawInspector();
	void OnSave(JSON_Object* obj);

private:

	uint uid = 0; 
	bool open_graph = false;
	

};


#endif
