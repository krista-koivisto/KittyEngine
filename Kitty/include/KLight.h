/**
 * Kitty Engine
 * KLight.h
 *
 * Light object to light up scenes with.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KLIGHT_H
#define KENGINE_KLIGHT_H

#include <glm/glm.hpp>
#include "KEngine.h"

namespace Kitty
{
	class KLight
	{
	private:
		KScene *context = nullptr;

		glm::vec4 rotation;
		glm::vec3 position;
		glm::mat4 rotationMatrix;
		glm::mat4 translationMatrix;



	public:
		explicit KLight(KScene *contextScene);
		~KLight() = default;

		glm::vec3 color;
		float constantAttenuation = 0.0f;
		float linearAttenuation = 1.0f;
		float quadraticAttenuation = 0.0f;

		/**
		 * \brief Set light position.
		 *
		 * \param newPosition New position.
		 */
		void SetPosition(glm::vec3 newPosition);

		/**
		 * \brief Set light rotation.
		 *
		 * \param newRotation New rotation angle in degrees.
		 * \param axis Axis along which to rotate light (for example 0, 0, 1 to rotate around the vertical axis).
		 */
		void SetRotation(glm::vec3 axis, float newRotation);

		/**
		 * \brief Set light rotation.
		 *
		 * \param axis Axis along which to rotate light. X, y and z for axis and w for angle.
		 */
		void SetRotation(glm::vec4 newRotation);

		/**
		 * \brief Get light position.
		 *
		 * \return Light position.
		 */
		glm::vec3 GetPosition() { return position; }

		/**
		 * \brief Get light rotation.
		 *
		 * \return Object light, x, y, z for the axes and w for rotation degrees.
		 */
		glm::vec4 GetRotation() { return rotation; }

		/**
		 * \brief Get the lights's translation and rotation matrix.
		 *
		 * \return Light translation and rotation matrix.
		 */
		glm::mat4 GetLightMatrix() { return rotationMatrix * translationMatrix; }
	};
}


#endif //KENGINE_KLIGHT_H
