#ifndef _NODE_TRANSLATE_GAME_OBJECT_
#define _NODE_TRANSLATE_GAME_OBJECT_

#include "NodeGraph.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class NodeTranslateGameObject : public Node
{
public:

	NodeTranslateGameObject(int id);
	~NodeTranslateGameObject();

	bool Update(float dt);
	void DrawNode();
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);
	void SetReferencesNodeDuplicated(Node& node);
private:

	GameObject* go = nullptr;
	math::float3 translation = math::float3::zero;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_

