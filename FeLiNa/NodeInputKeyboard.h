#ifndef _NODE_INPUT_KEYBOARD_
#define _NODE_INPUT_KEYBOARD_

#include "NodeGraph.h"


class NodeInputKeyboard : public Node
{
public:

	NodeInputKeyboard(int id,char key = '1');
	bool Update();
	void DrawNode();
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);

private:

	int key_code = -1;
	char key;


};







#endif
