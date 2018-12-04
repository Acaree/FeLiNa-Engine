#include "NodeInputKeyboard.h"
#include "Application.h"
#include "ModuleInput.h"


NodeInputKeyboard::NodeInputKeyboard(char key) : Node(7, "Input Event", {100,100}, 0, 1, NodeType::EventType)
{
	this->key = key;
	this->key_code = SDL_GetScancodeFromName(&key);

}



bool NodeInputKeyboard::Update()
{
	bool ret = false;


	if (App->input->GetKey(key_code) == KEY_REPEAT)
	{
		ret = true;
	}


	return ret;
}

void NodeInputKeyboard::DrawNode()
{

	if (ImGui::InputText("Key react:", &key, 2))
	{
		key_code = SDL_GetScancodeFromName(&key);
	}

}