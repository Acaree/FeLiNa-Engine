#include "NodeMouseMotion.h"
#include "ModuleInput.h"
#include "Application.h"

NodeMouseMotion::NodeMouseMotion(): Node(9, "Mouse Motion", { 100,100 }, 0, 1, NodeType::FunctionType)
{

}

bool NodeMouseMotion::Update()
{
	bool ret = false;

	int x = App->input->GetMouseXMotion();
	int y = App->input->GetMouseYMotion();

	if (x != 0 || y != 0)
	{
		ret = true;



	}


	return ret;
}