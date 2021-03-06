#ifndef _NODE_INPUT_KEYBOARD_
#define _NODE_INPUT_KEYBOARD_

#include "NodeGraph.h"


class NodeInputKeyboard : public Node
{
public:

	NodeInputKeyboard(int id,char key = '1');
	~NodeInputKeyboard();
	bool Update(float dt);
	void DrawNode();
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);
	void SetReferencesNodeDuplicated(Node& node);
private:

	int key_code = -1;
	char key[2];


};







#endif
