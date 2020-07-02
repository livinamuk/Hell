#include "hellpch.h"
#include "AnimatedNode.h"

namespace HellEngine
{
	AnimatedNode::AnimatedNode(const char* name)
	{
		m_nodeName = name;
	}
	/*bool AnimatedNode::IsAnimated()
	{
		if (m_nodeKeys.size() > 0)
			return true;
		else
			return false;
	}*/
}