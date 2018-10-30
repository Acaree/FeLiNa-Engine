#include "Quadtree.h"
#include "GameObject.h"


QuadTreeNode::QuadTreeNode(const math::AABB &bounding_box, QuadTreeNode* parent)
{
	this->boundig_box = boundig_box;
	this->parent = parent;

	for (uint i = 0; i < 4; ++i)
		childrens[i] = nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
	for (uint i = 0; i < 4; ++i)
	{
		if (childrens[i] != nullptr)
		{
			childrens[i]->objects.clear();
			RELEASE(childrens[i]);
		}
	}

	if (parent != nullptr)
		RELEASE(parent);
}

void QuadTreeNode::Insert(GameObject * go)
{
	if (objects.size() < QUADTREE_MAX_ITEMS)
	{
		objects.push_back(go);
	}
	else
	{
		if (isLeaf())
			SubdivideNode();

		objects.push_back(go);
		DistributeChildrens();
	}
}

void QuadTreeNode::SubdivideNode()
{
	const math::float3 size_box = boundig_box.Size(); //Take the size of actual boundig box
	const math::float3 subdivision_size = { size_box.x*0.5F, size_box.y, size_box.z*0.5F }; //Calculate the new size for subdivision bounding box

	const math::float3 center_bounding_box = boundig_box.CenterPoint(); //Center of actual boundig box
	math::float3 box_child_center;
	math::AABB child_bounding_box;

	//Now create the new child's node from this node

	//TOP-LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x, center_bounding_box.y, center_bounding_box.z - subdivision_size.z };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//TOP-RIGHT 
	box_child_center = { center_bounding_box.x + subdivision_size.x, center_bounding_box.y, center_bounding_box.z - subdivision_size.z };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::UP_RIGHT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_LEFT 
	box_child_center = { center_bounding_box.x - subdivision_size.x, center_bounding_box.y, center_bounding_box.z + subdivision_size.z };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_LEFT] = new QuadTreeNode(child_bounding_box, this);

	//DOWN_RIGHT
	box_child_center = { center_bounding_box.x + subdivision_size.x, center_bounding_box.y, center_bounding_box.z + subdivision_size.z };
	child_bounding_box.SetFromCenterAndSize(box_child_center, subdivision_size);
	childrens[SubdivideChildsPosition::DOWN_RIGHT] = new QuadTreeNode(child_bounding_box, this);
}

bool QuadTreeNode::isLeaf() const
{
	return childrens[0] == nullptr;
}

template<class TYPE>
void QuadTreeNode::CollectIntersections(std::vector<GameObject*> &objects, const TYPE & primitive)
{
	if (primitive.Intersects(object))
	{
		for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++i)
		{
			if (primitive.Intersects((*it)->bounding_box))
				objects.push_back(*it);
		}
		
		if (!isLeaf) 
		{
			for (int i = 0; i < 4; ++i)
			{
				if (childrens[i] != nullptr)
					childrens[i]->CollectIntersections(objects, primitive);
			}
		}
	}

}

void QuadTreeNode::DistributeChildrens()
{
	std::list<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end())
	{
		//NEED THINK FROM HOW TO DO THIS :/

	}
}