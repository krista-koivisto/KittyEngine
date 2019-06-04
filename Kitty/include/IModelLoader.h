/**
 * Kitty Engine
 * IModelLoader.h
 *
 * This is a virtual interface for implementing model loaders.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_IMODELLOADER_H
#define KENGINE_IMODELLOADER_H

#include "KEngine.h"
#include "Vulkan/KVulkanBuffer.h"
#include "KObject.h"
#include "KInstancedObject.h"

namespace Kitty
{
	class KObject;
	class KInstancedObject;

	class IModelLoader
	{
	private:

	public:
		/**
		 * \brief Load model as object.
		 *
		 * \param filename Filename of the image to load.
		 * \return Created object containing the loaded model mesh.
		 */
		virtual KObject *LoadModel(std::string filename) = 0;

		/**
		 * \brief Set the Vulkan context.
		 *
		 * This is used when the user passes a custom model loader to the engine, because
		 * the user does not have access to the engine Vulkan instance.
		 *
		 * \param vulkanContext Pointer to a Vulkan context.
		 */
		virtual void SetVulkanContext(Vulkan::KVulkan *vulkanContext) = 0;

		/**
		 * \brief Clear the object loader's mesh cache.
		 *
		 * Model loaders should implement a mesh cache to make loading already loaded
		 * meshes quicker. This function clears the cache entirely.
		 */
		virtual void ClearCache() = 0;

		/**
		 * \brief Remove a single mesh from the cache.
		 *
		 * This function should be called whenever a mesh is destroyed to make sure
		 * the loader doesn't try to access the removed mesh later.
		 *
		 * \param mesh Pointer to mesh which needs to be removed.
		 */
		virtual void RemoveFromCache(KMesh *mesh) = 0;
	};
}


#endif //KENGINE_IMODELLOADER_H
