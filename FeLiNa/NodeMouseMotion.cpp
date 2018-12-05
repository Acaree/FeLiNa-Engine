#include "NodeMouseMotion.h"
#include "ModuleInput.h"
#include "Application.h"

NodeMouseMotion::NodeMouseMotion(int id): Node(id, "Mouse Motion", { 100,100 }, 0, 1, NodeType::EventType)
{

}

bool NodeMouseMotion::Update()
{
	returned_result = false;

	int x = App->input->GetMouseXMotion();
	int y = App->input->GetMouseYMotion();

	if (x != 0 || y != 0)
	{
		returned_result = true;

	}


	return returned_result;
}