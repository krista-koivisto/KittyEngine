/**
 * Kitty engine Vulkan implementation
 * KVulkanDescriptorPool.cpp
 *
 * Vulkan descriptor pool implementation for the Kitty graphics
 * engine. This functions as an abstraction layer between Vulkan
 * and the Kitty engine, direct access from the end user
 * interface should never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanDescriptorPool.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkanDescriptorPool::KVulkanDescriptorPool(KVulkan *mainContext, std::vector<VkDescriptorPoolSize> poolSizes)
		{
			context = mainContext;

			uint32_t maxSets = 0;

			for (auto size : poolSizes)
			{
				maxSets += size.descriptorCount;
			}

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = maxSets;

			if (vkCreateDescriptorPool(context->device->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_DESC_POOL_FAIL));
			}
		}

		bool KVulkanDescriptorPool::InitializeBinding(VkDescriptorSetLayoutBinding *binding,
		                                                VkDescriptorSetLayout *layout)
		{
			// Descriptor set and pipeline layouts
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
			VkDescriptorSetLayoutCreateInfo descriptorLayout = {};

			setLayoutBindings.push_back(*binding);
			descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayout.pBindings = setLayoutBindings.data();
			descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());

			if (vkCreateDescriptorSetLayout(context->device->device, &descriptorLayout, nullptr, layout))
			{
				return false;
			}

			return true;
		}

		void KVulkanDescriptorPool::AllocateDescriptor(VkDescriptorSetLayout *layout,
		                                               VkDescriptorSet *descriptorSet,
		                                               VkDescriptorType type,
		                                               VkDescriptorImageInfo *imageInfo,
		                                               VkDescriptorBufferInfo *bufferInfo,
		                                               uint32_t binding,
		                                               uint32_t descCount)
		{
			VkDescriptorSetLayout descLayout[] = {*layout};
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = descLayout;

			if (vkAllocateDescriptorSets(context->device->device, &allocInfo, descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_DESC_SET_FAIL));
			}

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = *descriptorSet;
			descriptorWrite.dstBinding = binding;
			descriptorWrite.descriptorType = type;
			descriptorWrite.descriptorCount = descCount;
			descriptorWrite.pImageInfo = imageInfo;
			descriptorWrite.pBufferInfo = bufferInfo;
			writeDescriptorSets.push_back(descriptorWrite);

			vkUpdateDescriptorSets(context->device->device, static_cast<uint32_t>(writeDescriptorSets.size()),
			                       writeDescriptorSets.data(), 0, nullptr);
		}

		KVulkanDescriptorPool::~KVulkanDescriptorPool()
		{
			vkDestroyDescriptorPool(context->device->device, descriptorPool, nullptr);
		}
	}
}
