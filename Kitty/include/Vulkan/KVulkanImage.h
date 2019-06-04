/**
 * Kitty engine Vulkan implementation
 * KVulkanImage.h
 *
 * Vulkan image implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANIMAGE_H
#define KENGINE_KVULKANIMAGE_H


#include "KVulkan.h"
#include "KVulkanBuffer.h"

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkanImage
		{
		private:
			KVulkan *context = nullptr;

			KError Initialize(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
						  VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		public:
			/**
			 * \brief Create a Vulkan image.
			 *
			 * Create a Vulkan image with the given parameters. Looks scary, I know. But it's not,
			 * I promise. Don't worry! :)
			 *
			 * \param mainContext Vulkan context to bind image to.
			 * \param width Width of the image to be created.
			 * \param height Height of the image to be created.
			 * \param format Format of the image to be created.
			 * \param tiling What do we want to do when tiling happens?
			 * \param usage Usage parameters for the image.
			 * \param properties Memory properties of the image.
			 */
			explicit KVulkanImage(KVulkan *mainContext, uint32_t width, uint32_t height, VkFormat format,
			                      VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
			~KVulkanImage();

			/**
			 * \brief Function for transitioning one image layout to another.
			 *
			 * \param image Vulkan image to be transitioned.
			 * \param format Image format.
			 * \param oldLayout Old image layout.
			 * \param newLayout Desired image layout.
			 */
			void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

			VkImage image = {};
			VkDeviceMemory imageMemory = {};
		};
	}
}


#endif //KENGINE_KVULKANIMAGE_H
