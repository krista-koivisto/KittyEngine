/**
 * Kitty Engine
 * ITextureLoader.h
 *
 * This is a virtual interface for implementing texture loaders.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_ITEXTURELOADER_H
#define KENGINE_ITEXTURELOADER_H

#include "KEngine.h"
#include "Vulkan/KVulkanBuffer.h"
#include "KMaterial.h"

namespace Kitty
{
	class KMaterial;

	class ITextureLoader
	{
	private:

	public:
		/**
		 * \brief Load an image from a file into a texture.
		 *
		 * \param filename File to load image data from.
		 * \param prop Which layer do we want to assign this to? Use KT_PROP_DIFFUSE for a "regular" texture.
		 * \return Created material using the loaded image texture.
		 */
		virtual KMaterial *LoadImage(std::string filename, KE_TEXTURE_PROPERTY prop) = 0;

		/**
		 * \brief Set the Vulkan context.
		 *
		 * This is used when the user passes a custom texture loader to the engine, because
		 * the user does not have access to the engine Vulkan instance.
		 *
		 * \param vulkanContext Pointer to a Vulkan context.
		 */
		virtual void SetVulkanContext(Vulkan::KVulkan *vulkanContext) = 0;
	};
}


#endif //KENGINE_ITEXTURELOEADER_H
