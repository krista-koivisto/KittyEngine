/**
 * Kitty engine Vulkan implementation
 * KVulkanGraphicsPipeline.cpp
 *
 * Vulkan graphics pipeline implementation for the Kitty graphics
 * engine. This functions as an abstraction layer between Vulkan
 * and the Kitty engine, direct access from the end user interface
 * should never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanGraphicsPipeline.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanGraphicsPipeline::Initialize(KVulkanGraphicsSettings *createInfo)
		{
			KVulkanGraphicsSettings info = defaults.ObtainValues(createInfo, &defaults.graphicsPipelineInfo);

			if (!LoadShaders(createInfo))
			{
				return KE_VULKAN_SHADER_FAIL;
			}

			if (!CreatePipeline(shaderStages, &info))
			{
				return KE_VULKAN_GPIPELINE_FAIL;
			}

			// No longer needed
			for (auto shader : vertShaders) vkDestroyShaderModule(context->device->device, shader, nullptr);
			for (auto shader : fragShaders) vkDestroyShaderModule(context->device->device, shader, nullptr);

			return KE_OK;
		}

		bool KVulkanGraphicsPipeline::LoadShaders(KVulkanGraphicsSettings *createInfo)
		{
			for (const auto &vxShader : createInfo->vertexShaders)
			{
				InitializeShaderData(vxShader, VK_SHADER_STAGE_VERTEX_BIT);
			}

			for (const auto &fgShader : createInfo->fragmentShaders)
			{
				InitializeShaderData(fgShader, VK_SHADER_STAGE_FRAGMENT_BIT);
			}

			// Make sure some shaders were loaded if any were requested.
			return ((!vertShaders.empty() || createInfo->vertexShaders.empty()) && (!fragShaders.empty() ||
					createInfo->fragmentShaders.empty()));
		}

		void KVulkanGraphicsPipeline::InitializeShaderData(std::string filename, VkShaderStageFlagBits shaderStage)
		{
			auto *helper = new KHelper();

			auto code = helper->ReadBinaryFile(filename);
			auto module = CreateShaderModule(code);

			if (!code.empty() && shaderStage == VK_SHADER_STAGE_VERTEX_BIT) vertShaders.push_back(module);
			else if (!code.empty() && shaderStage == VK_SHADER_STAGE_FRAGMENT_BIT) fragShaders.push_back(module);
			else throw std::runtime_error(WhatWentWrong(KE_VULKAN_SHADER_FAIL));

			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = shaderStage;
			vertShaderStageInfo.module = module;
			vertShaderStageInfo.pName = "main";

			shaderStages.push_back(vertShaderStageInfo);

			delete (helper);
		}

		VkShaderModule KVulkanGraphicsPipeline::CreateShaderModule(std::vector<char> &code)
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(context->device->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_SHADER_FAIL));
			}

			return shaderModule;
		}

		bool KVulkanGraphicsPipeline::CreatePipeline(std::vector<VkPipelineShaderStageCreateInfo> shaderStages,
		                                             KVulkanGraphicsSettings *createInfo)
		{
			KVulkanGraphicsSettings info = defaults.ObtainValues(createInfo, &defaults.graphicsPipelineInfo);

			auto swapChainExtent = context->swapChain->swapChainExtent;
			auto device = context->device->device;

			auto vertexInputInfo = defaults.ObtainValues(&info.vertexInputInfo, &defaults.vertexInputInfo);
			auto inputAssembly = defaults.ObtainValues(&info.inputAssembly, &defaults.inputAssembly);
			auto viewport = defaults.ObtainValues(&info.viewport, &defaults.viewport);
			if (!viewport.width) viewport.width = (float) swapChainExtent.width;
			if (!viewport.height) viewport.height = (float) swapChainExtent.height;

			auto scissor = defaults.ObtainValues(&info.scissor, &defaults.scissor);
			if (scissor.extent.width == 0 && scissor.extent.height == 0) scissor.extent = swapChainExtent;

			auto viewportState = defaults.ObtainValues(&info.viewportState, &defaults.viewportState);
			if (!viewportState.pViewports) viewportState.pViewports = &viewport;
			if (!viewportState.pScissors) viewportState.pScissors = &scissor;

			auto rasterizer = defaults.ObtainValues(&info.rasterizer, &defaults.rasterizer);
			auto multisampling = defaults.ObtainValues(&info.multisampling, &defaults.multisampling);
			auto colorBlendAttachment = defaults.ObtainValues(&info.colorBlendAttachment, &defaults.colorBlendAttachment);
			auto colorBlending = defaults.ObtainValues(&info.colorBlending, &defaults.colorBlending);
			auto depthStencil = defaults.ObtainValues(&info.depthStencil, &defaults.depthStencil);
			if (!colorBlending.pAttachments) colorBlending.pAttachments = &colorBlendAttachment;

			auto pushConstantInfo = defaults.ObtainValues(&info.pushConstantRange, &defaults.pushConstantRange);

			if (pushConstantInfo.size == 0)
			{
				pushConstantInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				pushConstantInfo.offset = 0;
				pushConstantInfo.size = sizeof(KVulkanPushConstants);
			}

			auto pipelineLayoutInfo = defaults.ObtainValues(&info.pipelineLayoutInfo, &defaults.pipelineLayoutInfo);

			if (pipelineLayoutInfo.pPushConstantRanges == nullptr && pushConstantInfo.size)
			{
				pipelineLayoutInfo.pushConstantRangeCount = 1;
				pipelineLayoutInfo.pPushConstantRanges = &pushConstantInfo;
			}

			if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				return KE_VULKAN_GPIPELINE_FAIL;
			}

			auto pipelineInfo = defaults.ObtainValues(&info.pipelineInfo, &defaults.pipelineInfo);
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.layout = pipelineLayout;

			if (info.dynamicState.dynamicStateCount)
			{
				pipelineInfo.pDynamicState = &info.dynamicState;
			}

			return !(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
			         VK_SUCCESS);
		}

		KVulkanGraphicsPipeline::~KVulkanGraphicsPipeline()
		{
			VkDevice device = context->device->device;

			vkDestroyPipeline(device, graphicsPipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		}
	}
}
