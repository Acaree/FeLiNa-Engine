#ifndef _NODE_ROTATE_GAME_OBJECT_
#define _NODE_ROTATE_GAME_OBJECT_

#include "NodeGraph.h"


class GameObject;

class NodeRotateGameObject : public Node
{
public:
	NodeRotateGameObject(int id);
	bool Update();
	void DrawNode();

private:
	GameObject* go = nullptr;
};



#endif
