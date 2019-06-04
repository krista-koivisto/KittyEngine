/**
 * Kitty engine Vulkan implementation
 * KVulkanRenderPass.cpp
 *
 * Vulkan render pass implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanRenderPass.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkanRenderPass::KVulkanRenderPass(KVulkan *mainContext, KVulkanGraphicsSettings *createInfo)
		{
			context = mainContext;

			KVulkanGraphicsSettings info = defaults.ObtainValues(createInfo, &defaults.graphicsPipelineInfo);

			auto colorAttach = defaults.ObtainValues(&info.colorAttachment, &defaults.colorAttachment);
			if (!colorAttach.format) colorAttach.format = context->swapChain->swapChainImageFormat;

			auto colorAttachRef = defaults.ObtainValues(&info.colorAttachmentRef, &defaults.colorAttachmentRef);

			auto depthAttachment = defaults.ObtainValues(&info.depthAttachment, &defaults.depthAttachment);
			depthAttachment.format = context->device->features.depthFormat;

			auto depthAttachmentRef = defaults.ObtainValues(&info.depthAttachmentRef, &defaults.depthAttachmentRef);

			auto dependency = defaults.ObtainValues(&info.dependency, &defaults.dependency);

			auto subpassDesc = defaults.ObtainValues(&info.subpass, &defaults.subpass);
			if (!subpassDesc.pColorAttachments)
			{
				subpassDesc.pColorAttachments = &colorAttachRef;
				subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;
			}

			std::array<VkAttachmentDescription, 2> attachments = {colorAttach, depthAttachment};

			auto renderPassInfo = defaults.ObtainValues(&info.renderPassCreateInfo, &defaults.renderPassCreateInfo);
			if (!renderPassInfo.pAttachments)
			{
				renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				renderPassInfo.pAttachments = attachments.data();
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpassDesc;
				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;
			}

			if (vkCreateRenderPass(context->device->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_RENDERPASS_FAIL));
			}
		}

		KVulkanRenderPass::~KVulkanRenderPass()
		{
			vkDestroyRenderPass(context->device->device, renderPass, nullptr);
		}
	}
}
