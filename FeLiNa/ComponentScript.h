#ifndef _COMPONENT_SCRIPT_
#define _COMPONENT_SCRIPT_

#include "Globals.h"
#include "NodeGraph.h"
#include "Component.h"

class GameObject;

class ComponentScript : public Component
{
public:

	ComponentScript(GameObject* parent);
	~ComponentScript();

	void DrawInspector();

private:

	NodeGraph* graph = nullptr;
	bool open_graph = false;

};


#endif
