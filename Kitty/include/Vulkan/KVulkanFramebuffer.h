/**
 * Kitty engine Vulkan implementation
 * KVulkanFramebuffer.h
 *
 * Vulkan framebuffer implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANFRAMEBUFFER_H
#define KENGINE_KVULKANFRAMEBUFFER_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"
#include "KVulkanDefaults.h"
#include "../KError.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanFramebuffer
		{
		private:
			KVulkan *context = nullptr;
			KVulkanDefaults defaults;

		public:
			explicit KVulkanFramebuffer(KVulkan *mainContext) : context(mainContext) {}
			~KVulkanFramebuffer();

			std::vector<VkFramebuffer> swapChainFramebuffers;

			/**
			 * \brief Initialize the frame buffer instance.
			 *
			 * \param createInfo [optional] Custom settings for the frame buffer.
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(VkFramebufferCreateInfo *createInfo = nullptr);
		};
	}
}


#endif //KENGINE_KVULKANFRAMEBUFFER_H
