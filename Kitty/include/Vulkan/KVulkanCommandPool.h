/**
 * Kitty engine Vulkan implementation
 * KVulkanFramebuffer.h
 *
 * Vulkan command pool implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANCOMMANDPOOL_H
#define KENGINE_KVULKANCOMMANDPOOL_H

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

		class KVulkanCommandPool
		{
		private:
			KVulkan *context;
			KVulkanDefaults defaults;

			/**
			 * \brief Create command buffers.
			 *
			 * \param allocInfo [optional] Custom command buffer settings.
			 * \return true on success, false on fail.
			 */
			bool CreateCommandBuffers(VkCommandBufferAllocateInfo *allocInfo = nullptr);

		public:
			explicit KVulkanCommandPool(KVulkan *mainContext, KVulkanCommandSettings *settings);
			~KVulkanCommandPool();

			VkCommandPool commandPool = {};
			std::vector<VkCommandBuffer> commandBuffers = {};
			VkRenderPassBeginInfo renderPassBeginInfo = {};

			/**
			 * \brief Initialize the graphics command pool.
			 *
			 * \param Custom command settings and callbacks.
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeGraphicsBuffer(KVulkanCommandSettings *settings = nullptr);

			/**
			 * \brief Initialize the transfer command pool.
			 *
			 * \param Custom command settings and callbacks.
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeTransferBuffer(KVulkanCommandSettings *settings = nullptr);

			/**
			 * \brief Initiate a single execution command buffer.
			 *
			 * \return Initiated command buffer.
			 */
			VkCommandBuffer InitiateCommand();

			/**
			 * \brief Finalize an initiated single execution command buffer.
			 *
			 * \param commandBuffer Command buffer to finalize.
			 * \param queue Pointer to queue which to send it to (for example context->device->graphicsQueue).
			 */
			void FinalizeCommand(VkCommandBuffer commandBuffer, VkQueue queue);
		};
	}
}


#endif //KENGINE_KVULKANCOMMANDPOOL_H
