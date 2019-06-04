/**
 * Kitty engine
 * KInstancedObject.h
 *
 * Instanced objects class.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KINSTANCEDOBJECT_H
#define KENGINE_KINSTANCEDOBJECT_H

#include "IObject.h"
#include "Vulkan/KVulkan.h"
#include <glm/glm.hpp>

namespace Kitty
{
	class KInstancedObject
	{
	private:
		IObject *instanceParent;
		Vulkan::InstanceData instanceData;

	public:
		explicit KInstancedObject(IObject *parent);

		/**
		 * \brief Set instance position.
		 *
		 * \param newPosition New position.
		 */
		void SetPosition(glm::vec3 newPosition);

		/**
		 * \brief Get instance data for updating the vertex buffer.
		 *
		 * \return Instance data.
		 */
		Vulkan::InstanceData GetInstanceData() { return instanceData; };

		/**
		 * \brief Get the parent object from which this instance was created.
		 *
		 * \return Instance's parent object.
		 */
		IObject *GetParent() { return instanceParent; };
	};
}


#endif //KENGINE_KINSTANCEDOBJECT_H
