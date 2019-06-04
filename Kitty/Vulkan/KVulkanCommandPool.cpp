/**
 * Kitty engine Vulkan implementation
 * KVulkanFramebuffer.cpp
 *
 * Vulkan command pool implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanCommandPool.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanCommandPool::InitializeGraphicsBuffer(KVulkanCommandSettings *settings)
		{
			if (!CreateCommandBuffers(&settings->allocInfo))
			{
				return KE_VULKAN_CMDBUFFERS_FAIL;
			}

			for (size_t i = 0; i < commandBuffers.size(); i++)
			{
				auto cmdBufferBegin = defaults.ObtainValues(&settings->graphicsCmdBufferInfo,
				                                            &defaults.graphicsCmdBufferInfo);

				renderPassBeginInfo = defaults.ObtainValues(&settings->renderPassInfo, &defaults.renderPassInfo);

				// Allow users to override the entire command pool
				if (settings->graphicsCmdPoolOverride != nullptr)
				{
					settings->graphicsCmdPoolOverride(commandBuffers[i],
					                             context->mainPipeline->graphicsPipeline,
					                             context->mainRenderPass->renderPass,
					                             context->frameBuffer->swapChainFramebuffers[i],
					                             context->swapChain->swapChainExtent);
				}
				else
				{
					vkBeginCommandBuffer(commandBuffers[i], &cmdBufferBegin);

					if (!renderPassBeginInfo.renderPass)
					{
						renderPassBeginInfo.renderPass = context->mainRenderPass->renderPass;
					}

					if (!renderPassBeginInfo.framebuffer)
					{
						renderPassBeginInfo.framebuffer = context->frameBuffer->swapChainFramebuffers[i];
					}

					if (!renderPassBeginInfo.renderArea.extent.height && !renderPassBeginInfo.renderArea.extent.width)
					{
						renderPassBeginInfo.renderArea.extent = context->swapChain->swapChainExtent;
					}

					vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

					if (settings->sceneStaticRenderCallback != nullptr)
					{
						settings->sceneStaticRenderCallback(commandBuffers[i]);
					}

					vkCmdEndRenderPass(commandBuffers[i]);

					if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
					{
						throw std::runtime_error(WhatWentWrong(KE_VULKAN_CMDBUFFERS_FAIL));
					}
				}
			}

			return KE_OK;
		}

		KError KVulkanCommandPool::InitializeTransferBuffer(KVulkanCommandSettings *settings)
		{
			if (!CreateCommandBuffers(&settings->allocInfo))
			{
				return KE_VULKAN_CMDBUFFERS_FAIL;
			}

			return KE_OK;
		}

		bool KVulkanCommandPool::CreateCommandBuffers(VkCommandBufferAllocateInfo *allocInfo)
		{
			commandBuffers.resize(context->frameBuffer->swapChainFramebuffers.size());

			auto allocationInfo = defaults.ObtainValues(allocInfo, &defaults.bufferAllocationInfo);
			if (!allocationInfo.commandBufferCount) allocationInfo.commandPool = commandPool;
			if (!allocationInfo.commandBufferCount) allocationInfo.commandBufferCount = (uint32_t) commandBuffers.size();

			return !(vkAllocateCommandBuffers(context->device->device, &allocationInfo, commandBuffers.data()) !=
			         VK_SUCCESS);

		}

		VkCommandBuffer KVulkanCommandPool::InitiateCommand()
		{
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(context->device->device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		void KVulkanCommandPool::FinalizeCommand(VkCommandBuffer commandBuffer, VkQueue queue)
		{
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(queue);

			vkFreeCommandBuffers(context->device->device, commandPool, 1, &commandBuffer);
		}

		KVulkanCommandPool::~KVulkanCommandPool()
		{
			vkFreeCommandBuffers(context->device->device, commandPool, static_cast<uint32_t >(commandBuffers.size()),
			                     commandBuffers.data());
			vkDestroyCommandPool(context->device->device, commandPool, nullptr);
		}

		KVulkanCommandPool::KVulkanCommandPool(KVulkan *mainContext, KVulkanCommandSettings *settings)
		{
			context = mainContext;

			VkCommandPoolCreateInfo createInfo = defaults.ObtainValues(&settings->poolInfo, &defaults.poolInfo);
			if (!createInfo.queueFamilyIndex == UINT32_MAX)
			{
				createInfo.queueFamilyIndex = context->device->features.graphicsFamily;
			}

			if (vkCreateCommandPool(context->device->device, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_CMDPOOL_FAIL));
			}
		}
	}
}
