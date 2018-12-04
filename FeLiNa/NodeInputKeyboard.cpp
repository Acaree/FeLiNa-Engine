#include "NodeInputKeyboard.h"
#include "Application.h"
#include "ModuleInput.h"

NodeInputKeyboard::NodeInputKeyboard(char key) : Node(7, "Input Event", {100,100}, 0, 1, NodeType::EventType)
{
	this->key = key;
	this->key_code = SDL_GetKeyFromName(&key);

}

void NodeInputKeyboard::DrawNode()
{

	if (ImGui::InputText("Key react:", &key, 2))
	{
		key_code = SDL_GetKeyFromName(&key);
	}

}