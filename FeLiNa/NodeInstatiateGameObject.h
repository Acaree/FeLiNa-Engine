#ifndef _NODE_INSTATIATE_GAME_OBJECT_
#define _NODE_INSTATIATE_GAME_OBJECT_

#include "NodeGraph.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class NodeInstatiateGameObject : public Node
{
public:

	NodeInstatiateGameObject(int id);
	bool Update();
	void DrawNode();

private:

	GameObject* go = nullptr;
	math::float3 new_pos = math::float3::zero;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_