/**
 * Kitty engine Vulkan implementation
 * KVulkanRenderPass.h
 *
 * Vulkan render pass implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANRENDERPASS_H
#define KENGINE_KVULKANRENDERPASS_H


#include "KVulkan.h"

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanRenderPass
		{
		private:
			KVulkan *context;
			KVulkanDefaults defaults;

		public:
			/**
			 * \brief Create the Vulkan render pass instance.
			 *
			 * \param mainContext Vulkan context from which to create render pass.
			 * \param createInfo [optional] Render pass settings (usually passed to Initialize()).
			 * \return true on success, false of fail.
			 */
			explicit KVulkanRenderPass(KVulkan *mainContext, KVulkanGraphicsSettings *createInfo = nullptr);
			~KVulkanRenderPass();

			VkRenderPass renderPass = {};
		};
	}
}


#endif //KENGINE_KVULKANRENDERPASS_H
