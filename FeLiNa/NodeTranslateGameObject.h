#ifndef _NODE_TRANSLATE_GAME_OBJECT_
#define _NODE_TRANSLATE_GAME_OBJECT_

#include "NodeGraph.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class NodeTranslateGameObject : public Node
{
public:

	NodeTranslateGameObject();
	void DrawNode();

private:

	GameObject* go = nullptr;
	math::float3 translation = math::float3::zero;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_

