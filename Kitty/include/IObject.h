/**
 * Kitty engine
 * IObject.h
 *
 * This class is a generic scene node, it just has a more newbie friendly name because
 * friendly names are nice. :)
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_IOBJECT_H
#define KENGINE_IOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Kitty
{
	class KScene;
	class KMesh;
	class KEngine;
	class KMaterial;
	class KObject;
	class KInstancedObject;

	class IObject
	{
	protected:
		KMesh *mesh = nullptr;
		KScene *context = nullptr;
		KMaterial *mat = nullptr;

		glm::vec4 rotation;
		glm::vec3 position;
		glm::vec3 scale;
		glm::mat4 rotationMatrix;
		glm::mat4 translationMatrix;
		glm::mat4 scaleMatrix;

		uint32_t instanceCount = 0;
		uint32_t index = 0;

	public:
		IObject() = default;
		~IObject() = default;

		/**
		 * \brief Create a new instance of this object.
		 *
		 * Creates a new instance of this object. It is much faster to create new instances
		 * of objects when you want many objects using the same mesh.
		 *
		 * Typically, if you want much more than one of any model you want to instance it.
		 *
		 * \return Pointer to instance created from the model.
		 */
		virtual KInstancedObject *CreateInstance();

		/**
		 * \brief Set object position.
		 *
		 * \param newPosition New position.
		 */
		virtual void SetPosition(glm::vec3 newPosition);

		/**
		 * \brief Set object scale.
		 *
		 * \param newScale New scale.
		 */
		virtual void SetScale(glm::vec3 newScale);

		/**
		 * \brief Set object scale.
		 *
		 * \param newScale New scale.
		 */
		virtual void SetScale(float newScale);

		/**
		 * \brief Set object rotation.
		 *
		 * \param newRotation New rotation angle in degrees.
		 * \param axis Axis along which to rotate object (for example 0, 0, 1 to rotate around the vertical axis).
		 */
		virtual void SetRotation(glm::vec3 axis, float newRotation);

		/**
		 * \brief Set object rotation.
		 *
		 * \param axis Axis along which to rotate object. X, y and z for axis and w for angle.
		 */
		virtual void SetRotation(glm::vec4 newRotation);

		/**
		 * \brief Assign a material to the object.
		 *
		 * \param material Pointer to the material.
		 */
		virtual void SetMaterial(KMaterial *material);

		/**
		 * \brief Set scene object tracker index.
		 *
		 * \param objectIndex Object index in scene object tracker.
		 */
		virtual void SetIndex(uint32_t objectIndex);

		/**
		 * \brief Get object mesh data.
		 *
		 * \return Mesh object.
		 */
		KMesh *GetMesh() { return mesh; }

		/**
		 * \brief Get object material.
		 *
		 * \return Pointer to the object's material.
		 */
		KMaterial *GetMaterial() { return mat; }

		/**
		 * \brief Get object position.
		 *
		 * \return Object position.
		 */
		glm::vec3 GetPosition() { return position; }

		/**
		 * \brief Get object rotation.
		 *
		 * \return Object rotation, x, y, z for the axes and w for rotation degrees.
		 */
		glm::vec4 GetRotation() { return rotation; }

		/**
		 * \brief Get object scale.
		 *
		 * \return Object scale.
		 */
		glm::vec3 GetScale() { return scale; }

		/**
		 * \brief Get the object's model matrix.
		 *
		 * \return Model matrix.
		 */
		glm::mat4 GetModelMatrix() { return translationMatrix * rotationMatrix * scaleMatrix; }

		/**
		 * \brief Get the number of instances created by this object.
		 *
		 * \return Number of instances of this object.
		 */
		uint32_t GetInstanceCount() { return instanceCount; }

		/**
		 * \brief Get scene object tracker index.
		 *
		 * \return Object index in scene object tracker.
		 */
		uint32_t GetIndex() { return index; }
	};
}


#endif //KENGINE_IOBJECT_H
