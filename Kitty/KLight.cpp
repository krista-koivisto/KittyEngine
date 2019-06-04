/**
 * Kitty Engine
 * KLight.cpp
 *
 * Light object to light up scenes with.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KLight.h"

namespace Kitty
{
	KLight::KLight(KScene *contextScene)
	{
		context = contextScene;
	}

	void KLight::SetPosition(glm::vec3 newPosition)
	{
		position = newPosition;
		translationMatrix = glm::translate(translationMatrix, position);
	}

	void KLight::SetRotation(glm::vec4 newRotation)
	{
		rotation = newRotation;
		rotationMatrix = glm::rotate(glm::mat4(), rotation.w, glm::vec3(rotation.x, rotation.y, rotation.z));
	}

	void KLight::SetRotation(glm::vec3 axis, float newRotation)
	{
		SetRotation(glm::vec4(axis, newRotation));
	}
}
