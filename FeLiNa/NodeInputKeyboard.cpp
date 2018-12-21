#include "NodeInputKeyboard.h"
#include "Application.h"
#include "ModuleInput.h"
#include <string>

NodeInputKeyboard::NodeInputKeyboard(int id, char key) : Node( id, "Input Event", {100,100}, 0, 1, NodeType::EventType)
{
	this->key[0] = key;
	this->key[1] = '\n';

	this->key_code = SDL_GetScancodeFromName(&key);
	subtype = NodeSubType::InputKeyboard;

}

bool NodeInputKeyboard::Update(float dt)
{
	returned_result = false;


	if (App->input->GetKey(key_code) == KEY_REPEAT)
	{
		returned_result = true;
	}


	return returned_result;
}

void NodeInputKeyboard::DrawNode()
{

	if (ImGui::InputText("Key react:", &key[0], 2))
	{
	
		key_code = SDL_GetScancodeFromName(&key[0]);
	}

}

void NodeInputKeyboard::SetNodeReferencesInJSON(JSON_Object* obj)
{
	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "key", key_code);
}

void NodeInputKeyboard::GetNodeReferencesInJSON(JSON_Object* obj)
{
	key_code = json_object_get_number(obj, "key");

	const char* c = SDL_GetScancodeName((SDL_Scancode)key_code);

	strcpy(key, c);
}

void NodeInputKeyboard::SetReferencesNodeDuplicated(Node& node)
{
	strcpy(key, ((NodeInputKeyboard*)&node)->key);
	key_code = ((NodeInputKeyboard*)&node)->key_code;
}