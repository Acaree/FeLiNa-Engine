#ifndef _NODE_INPUT_MOUSE_
#define _NODE_INPUT_MOUSE_

#include "NodeGraph.h"



class NodeInputMouse : public Node
{
public:

	NodeInputMouse(int id);

	bool Update();
	void DrawNode();
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);

private:
	int key_code = 0;
};




#endif

