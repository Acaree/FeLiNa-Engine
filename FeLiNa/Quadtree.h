#ifndef _QUADTREE_
#define _QUADTREE_

#include "Globals.h"
#include "MathGeoLib/Geometry/AABB.h"
#include <list>
#include <vector>

#include "GameObject.h"

#define QUADTREE_MAX_ITEMS 1 //1 for best testing

enum SubdivideChildsPosition //Position to subdivide childs
{
	UP_LEFT = 0,
	UP_RIGHT,
	DOWN_LEFT,
	DOWN_RIGHT
};

class QuadTreeNode
{
public:

	QuadTreeNode(const math::AABB &bounding_box , QuadTreeNode* parent);
	~QuadTreeNode();
	
	void Clear();

	void Insert(GameObject* go); // insert a go in this node.
	void SubdivideNode(); // create the childs
	bool isLeaf() const;

	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive);
	void CollectObjects(std::vector<GameObject*>&objects) const;

	void DistributeChildrens(); //order game objects depending on position to new childs.

	void DebugDraw();

public:
	
	QuadTreeNode* childrens[4]; //4 because it's a quadtree 
	QuadTreeNode* parent = nullptr; // parent of this node 

	std::vector<GameObject*> objects; // game objects inside this node.
	math::AABB bounding_box; // container of node to check the collisions

};


class QuadTree
{
public:

	QuadTree();
	~QuadTree();

	void SetBoundary(const math::AABB& boundary);
	void Insert(GameObject* go);
	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive);
	void CollectObjects(std::vector<GameObject*> &objects) const;
	
	void DebugDraw();

	void Clear();

public:
	QuadTreeNode* root_node = nullptr;
	uint size = 0;
};

template<typename TYPE>
inline void QuadTree::CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive)
{
	if (root_node != nullptr)
		root_node->CollectIntersections(objects, primitive);
}

template<typename TYPE>
inline void QuadTreeNode::CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive)
{
	if (primitive.Intersects(bounding_box))
	{
		for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			if (primitive.Intersects((*it)->bounding_box))
				objects.push_back(*it);
		}

		if (!isLeaf())
		{
			for (int i = 0; i < 4; ++i)
			{
				if (childrens[i] != nullptr)
					childrens[i]->CollectIntersections(objects, primitive);
			}
		}
	}

}





#endif
