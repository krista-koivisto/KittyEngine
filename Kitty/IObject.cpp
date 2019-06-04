/**
 * Kitty engine
 * IObject.cpp
 *
 * This class is a generic scene node, it just has a more newbie friendly name because
 * friendly names are nice. :)
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/IObject.h"
#include "include/KScene.h"

namespace Kitty
{
	KInstancedObject *IObject::CreateInstance()
	{
		instanceCount++;
		return context->AddObjectInstance(this);
	}

	void IObject::SetMaterial(KMaterial *material)
	{
		mat = material;
	}

	void IObject::SetPosition(glm::vec3 newPosition)
	{
		position = newPosition;
		translationMatrix = glm::translate(glm::mat4(), position);
	}

	void IObject::SetScale(glm::vec3 newScale)
	{
		scale = newScale;
		scaleMatrix = glm::scale(glm::mat4(), scale);
	}

	void IObject::SetScale(float newScale)
	{
		SetScale(glm::vec3(newScale, newScale, newScale));
	}

	void IObject::SetRotation(glm::vec4 newRotation)
	{
		rotation = newRotation;
		rotationMatrix = glm::rotate(glm::mat4(), rotation.w * 3.14159f / 180,
		                             glm::vec3(rotation.x, rotation.y, rotation.z));
	}

	void IObject::SetRotation(glm::vec3 axis, float newRotation)
	{
		SetRotation(glm::vec4(axis, newRotation));
	}

	void IObject::SetIndex(uint32_t objectIndex)
	{
		index = objectIndex;
	}
}