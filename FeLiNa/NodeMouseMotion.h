#ifndef _NODE_MOUSE_MOTION_
#define _NODE_MOUSE_MOTION_

#include "NodeGraph.h"


class NodeMouseMotion : public Node
{
public:

	NodeMouseMotion(int id);
	~NodeMouseMotion();
	bool Update(float dt);

};

#endif