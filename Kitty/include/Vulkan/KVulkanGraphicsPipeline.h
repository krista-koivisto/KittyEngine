/**
 * Kitty engine Vulkan implementation
 * KVulkanGraphicsPipeline.h
 *
 * Vulkan graphics pipeline implementation for the Kitty graphics
 * engine. This functions as an abstraction layer between Vulkan
 * and the Kitty engine, direct access from the end user interface
 * should never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANGRAPHICSPIPELINE_H
#define KENGINE_KVULKANGRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"
#include "KVulkanDefaults.h"
#include "KVulkanHelpers.h"
#include "../KHelper.h"
#include "../KError.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkanFramebuffer;

		class KVulkanGraphicsPipeline
		{
		private:
			KVulkan *context = nullptr;
			KVulkanDefaults defaults;

			std::vector<VkShaderModule> vertShaders;
			std::vector<VkShaderModule> fragShaders;
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

			/**
			 * \brief Load all needed shaders.
			 *
			 * \param createInfo Information on shaders to be loaded.
			 * \return true on success, false on fail.
			 */
			bool LoadShaders(KVulkanGraphicsSettings *createInfo);

			/**
			 * \brief Initialize shader data.
			 *
			 * Reads the required shader files, creates modules from them and fills in the
			 * shader stage info.
			 *
			 * \param filename Path to shader file to initialize.
			 * \param shaderStage Specify the shader stage. (e.g. VK_SHADER_STAGE_FRAGMENT_BIT)
			 */
			void InitializeShaderData(std::string filename, VkShaderStageFlagBits shaderStage);

		public:
			explicit KVulkanGraphicsPipeline(KVulkan *mainContext) : context(mainContext) {}
			~KVulkanGraphicsPipeline();

			VkPipelineLayout pipelineLayout = {};
			VkPipeline graphicsPipeline = {};

			/**
			 * \brief Initialize the graphics pipeline.
			 *
			 * \param createInfo [optional] Graphics pipeline creation options. There's a ton. :)
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(KVulkanGraphicsSettings *createInfo = nullptr);

			/**
			 * \brief Transfer SPIR-V shader code into a Vulkan module.
			 *
			 * \param code Compiled shader code in binary format.
			 * \return Vulkan shader module.
			 */
			VkShaderModule CreateShaderModule(std::vector<char> &code);

			/**
			 * \brief Create the graphics pipeline.
			 *
			 * \param shaderStages Shader stage creation info for the graphics pipeline.
			 * \param createInfo [optional] Settings to pass along to the graphics pipeline.
			 * \return true on success, false on fail.
			 */
			bool CreatePipeline(std::vector<VkPipelineShaderStageCreateInfo> shaderStages,
			                    KVulkanGraphicsSettings *createInfo = nullptr);
		};
	}
}


#endif //KENGINE_KVULKANGRAPHICSPIPELINE_H
