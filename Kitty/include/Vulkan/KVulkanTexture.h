/**
 * Kitty engine Vulkan implementation
 * KVulkanTexture.h
 *
 * Vulkan texture implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANTEXTURE_H
#define KENGINE_KVULKANTEXTURE_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"
#include "KVulkanBuffer.h"
#include "KVulkanImage.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanTexture
		{
		private:
			KVulkan *context;
			KVulkanSettings *settings = nullptr;

			/**
			 * \brief Copy 2D data from a Vulkan buffer to an image.
			 *
			 * \param buffer Vulkan buffer containing texels.
			 * \param image Vulkan image to copy the data to.
			 * \param width Width of the data.
			 * \param height Height of the data.
			 */
			void CopyFromBuffer2D(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

			/**
			 * \brief Create an image view to present the texture through to Vulkan.
			 */
			void CreateImageView();

			/**
			 * \brief Initialize a Vulkan texture sampler for the texture.
			 */
			void InitializeTextureSampler();

		public:
			explicit KVulkanTexture(KVulkan *mainContext, KVulkanSettings *vulkanSettings);
			~KVulkanTexture();

			VkSampler textureSampler = VK_NULL_HANDLE;
			VkImageView textureImageView = VK_NULL_HANDLE;
			KVulkanImage *image = nullptr;

			// TODO: Set things up so only a single command buffer is used by these functions.

			/**
			 * \brief Load 2D 8-bit RGBA pixel data into the texture.
			 *
			 * The data *must* be passed as 8-bit linear RGBA.
			 *
			 * \param buffer Linear 8-bit RGBA data.
			 * \param texWidth Texture width, or x dimension length.
			 * \param texHeight Texture height, or y dimenion length.
			 * \return KE_OK on success, error code on fail.
			 */
			KError SetImage2D_8R8G8B8A(unsigned char *buffer, uint32_t texWidth, uint32_t texHeight);
		};
	}
}


#endif //KENGINE_KVULKANTEXTURE_H
