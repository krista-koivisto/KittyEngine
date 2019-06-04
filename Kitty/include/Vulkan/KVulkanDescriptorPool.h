/**
 * Kitty engine Vulkan implementation
 * KVulkanDescriptorPool.h
 *
 * Vulkan descriptor pool implementation for the Kitty graphics
 * engine. This functions as an abstraction layer between Vulkan
 * and the Kitty engine, direct access from the end user
 * interface should never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANDESCRIPTORPOOL_H
#define KENGINE_KVULKANDESCRIPTORPOOL_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanDescriptorPool
		{
		private:
			KVulkan *context = nullptr;

		public:
			/**
			 * \brief Create a descriptor pool.
			 *
			 * \param mainContext Parent Vulkan context.
			 */
			explicit KVulkanDescriptorPool(KVulkan *mainContext, std::vector<VkDescriptorPoolSize> poolSizes);
			~KVulkanDescriptorPool();

			VkDescriptorPool descriptorPool = {};

			/**
			 * \brief Create a descriptor set layout from a binding.
			 *
			 * \param binding [in] Which binding do you want to use?
			 * \param layout [out] Which layout should be initialized?
			 * \return true on success, false on fail.
			 */
			bool InitializeBinding(VkDescriptorSetLayoutBinding *binding, VkDescriptorSetLayout *layout);

			/**
			 * \brief Allocate a descriptor from the descriptor pool.
			 *
			 * \param layout Set layout to be given to the descriptor.
			 * \param descriptorSet Descriptor set to which the descriptor will belong.
			 * \param type What does this descriptor describe?
			 * \param imageInfo Image info to pass if creating an image type. (Otherwise nullptr.)
			 * \param bufferInfo Buffer info to pass if creating a buffer type. (Otherwise nullptr.)
			 * \param binding To which binding on the set does it belong?
			 * \param descCount How many descriptors are you passing?
			 */
			void AllocateDescriptor(VkDescriptorSetLayout *layout,
			                        VkDescriptorSet *descriptorSet,
			                        VkDescriptorType type,
			                        VkDescriptorImageInfo *imageInfo,
			                        VkDescriptorBufferInfo *bufferInfo,
			                        uint32_t binding = 0,
			                        uint32_t descCount = 1);
		};
	}
}


#endif //KENGINE_KVULKANDESCRIPTORPOOL_H
