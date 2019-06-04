/**
 * Kitty engine
 * KInstancedObject.cpp
 *
 * Instanced objects class.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */


#include "include/KInstancedObject.h"

namespace Kitty
{
	KInstancedObject::KInstancedObject(IObject *parent)
	{
		instanceParent = parent;
		instanceData.rot = glm::vec3(0, 0, 0);
		instanceData.pos = glm::vec3(0, 0, 0);
		instanceData.scale = 1;
	}

	void KInstancedObject::SetPosition(glm::vec3 newPosition)
	{
		instanceData.pos = newPosition;
	}
}
