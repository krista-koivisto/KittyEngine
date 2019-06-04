/**
 * Kitty engine Vulkan implementation
 * KVulkanImageView.h
 *
 * Vulkan image view implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANIMAGEVIEW_H
#define KENGINE_KVULKANIMAGEVIEW_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"
#include "../KError.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanImageView
		{
		private:
			KVulkan *context;

		public:
			explicit KVulkanImageView(KVulkan *mainContext) : context(mainContext) {}
			~KVulkanImageView();

			VkImageView imageView = {};

			/**
			 * \brief Initialize the image view.
			 *
			 * Initialize the image view with the image and parameters passed.
			 *
			 * \param createInfo Information on how to create the image.
			 * \param image Pointer to the image itself.
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(VkImageViewCreateInfo createInfo, VkImage image);
		};
	}
}


#endif //KENGINE_KVULKANIMAGEVIEW_H
