/**
 * Kitty engine Vulkan implementation
 * KVulkanFramebuffer.cpp
 *
 * Vulkan framebuffer implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanFramebuffer.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanFramebuffer::Initialize(VkFramebufferCreateInfo *createInfo)
		{
			auto swapChainImageViews = context->swapChain->swapChainImageViews;
			auto swapChainExtent = context->swapChain->swapChainExtent;

			swapChainFramebuffers.resize(swapChainImageViews.size());

			for (size_t i = 0; i < swapChainImageViews.size(); i++)
			{
				std::array<VkImageView, 2> attachments = {
						swapChainImageViews[i]->imageView,
						context->depthImageView->imageView
				};

				auto framebufferInfo = defaults.ObtainValues(createInfo, &defaults.framebufferInfo);
				if (!framebufferInfo.renderPass) framebufferInfo.renderPass = context->mainRenderPass->renderPass;
				if (!framebufferInfo.width) framebufferInfo.width = swapChainExtent.width;
				if (!framebufferInfo.height) framebufferInfo.height = swapChainExtent.height;

				if (!framebufferInfo.pAttachments)
				{
					framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
					framebufferInfo.pAttachments = attachments.data();
				}

				if (vkCreateFramebuffer(context->device->device, &framebufferInfo, nullptr,
				                        &swapChainFramebuffers[i]) != VK_SUCCESS)
				{
					return KE_VULKAN_FRAMEBUFFER_FAIL;
				}
			}

			return KE_OK;
		}

		KVulkanFramebuffer::~KVulkanFramebuffer()
		{
			for (auto &swapChainFramebuffer : swapChainFramebuffers)
			{
				vkDestroyFramebuffer(context->device->device, swapChainFramebuffer, nullptr);
			}
		}
	}
}
