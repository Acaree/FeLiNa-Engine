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
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);
	void SetReferencesNodeDuplicated(Node& node);

private:

	math::float3 new_pos = math::float3::zero;
	math::float3 speed = math::float3::zero;
	std::string instance_fbx_path;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_
