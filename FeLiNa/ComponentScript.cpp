#include "ComponentScript.h"
#include "GameObject.h"


ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = Component_Script;
	graph = new NodeGraph();
}

ComponentScript::~ComponentScript()
{
	RELEASE(graph);
}

void ComponentScript::DrawInspector()
{


}