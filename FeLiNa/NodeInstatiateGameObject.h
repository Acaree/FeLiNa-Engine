#ifndef _NODE_INSTATIATE_GAME_OBJECT_
#define _NODE_INSTATIATE_GAME_OBJECT_

#include "NodeGraph.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class NodeInstatiateGameObject : public Node
{
public:

	NodeInstatiateGameObject(int id);
	bool Update(float dt);
	void DrawNode();
	void SetNodeReferencesInJSON(JSON_Object* obj);
	void GetNodeReferencesInJSON(JSON_Object* obj);
	void SetReferencesNodeDuplicated(Node& node);

private:

	math::float3 new_pos = math::float3::zero;
	math::float3 direction = math::float3::zero;
	float velocity = 0.0f;

	std::string instance_fbx_path;
	
	bool get_position_from_GO = false;
	std::string position_GO_name = "Game Object Invalid";
	GameObject* GO_position = nullptr;

	bool get_speed_dir_from_GO = false;
	std::string speed_dir_GO_name = "Game Object Invalid";
	GameObject* GO_speed_dir = nullptr;
	bool rectificate_x = false, rectificate_y = false, rectificate_z = false;
};



#endif // !_NODE_TRANSLATE_GAME_OBJECT_
