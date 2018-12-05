#ifndef _NODE_ROTATE_GAME_OBJECT_
#define _NODE_ROTATE_GAME_OBJECT_

#include "NodeGraph.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class NodeRotateGameObject : public Node
{
public:
	NodeRotateGameObject(int id);
	bool Update();
	void DrawNode();

private:
	GameObject* go = nullptr;
	math::float3 axis = math::float3::zero;
};



#endif
