#ifndef _NODE_INPUT_KEYBOARD_
#define _NODE_INPUT_KEYBOARD_

#include "NodeGraph.h"


class NodeInputKeyboard : public Node
{
public:

	NodeInputKeyboard(char key);

	void DrawNode();


private:
	int key_code = -1;
	char key;


};







#endif
