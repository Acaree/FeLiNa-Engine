#ifndef _NODE_TRANSLATE_GAME_OBJECT_
#define _NODE_TRANSLATE_GAME_OBJECT_

#include "NodeGraph.h"

class GameObject;

class NodeTranslateGameObject : public Node
{
public:

	NodeTranslateGameObject();
	void DrawNode();

private:

	GameObject* go = nullptr;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_

