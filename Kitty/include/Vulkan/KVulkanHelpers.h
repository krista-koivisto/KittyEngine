/**
 * Kitty engine Vulkan implementation helpers
 * KVulkanHelpers.h
 *
 * Helper objects as well as various "information packages."
 *
 * Because Vulkan gives us so many options it would make for too many
 * options to easily pass at once in the creation and setting up
 * stage. These information "packages" make life easier. This file
 * basically has a bunch of Vulkan TV dinners.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANHELPER_H
#define KENGINE_KVULKANHELPER_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <functional>
#include "KVulkanDefaults.h"

namespace Kitty
{
	namespace Vulkan
	{
		// Per-instance data block
		struct InstanceData
		{
			glm::vec3 pos;
			glm::vec3 rot;
			float scale;
		};

		struct Vertex
		{
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;
			glm::vec3 normal;

			//! Get vertex binding description (stride and input rate)
			static VkVertexInputBindingDescription getBindingDescription()
			{
				VkVertexInputBindingDescription bindingDescription = {};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			//! Get instance binding description (stride and input rate)
			static std::array<VkVertexInputBindingDescription, 2> getInstanceBindingDescription()
			{
				std::array<VkVertexInputBindingDescription, 2> bindingDescription = {};
				bindingDescription[0] = getBindingDescription();
				bindingDescription[1].binding = 1;
				bindingDescription[1].stride = sizeof(InstanceData);
				bindingDescription[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

				return bindingDescription;
			}

			//! Get vertex attributes (such as position and color).
			static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
			{
				std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = static_cast<uint32_t>(offsetof(Vertex, pos));

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = static_cast<uint32_t>(offsetof(Vertex, color));

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = static_cast<uint32_t>(offsetof(Vertex, texCoord));

				attributeDescriptions[3].binding = 0;
				attributeDescriptions[3].location = 3;
				attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[3].offset = static_cast<uint32_t>(offsetof(Vertex, normal));

				return attributeDescriptions;
			}

			//! Get instance attributes (such as position and color).
			static std::array<VkVertexInputAttributeDescription, 7> getInstanceAttributeDescriptions()
			{
				std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions = {};
				std::array<VkVertexInputAttributeDescription, 4> vxAttributes = getAttributeDescriptions();

				attributeDescriptions[0] = vxAttributes[0];
				attributeDescriptions[1] = vxAttributes[1];
				attributeDescriptions[2] = vxAttributes[2];
				attributeDescriptions[3] = vxAttributes[3];

				attributeDescriptions[4].binding = 1;
				attributeDescriptions[4].location = 4;
				attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[4].offset = 0;

				attributeDescriptions[5].binding = 1;
				attributeDescriptions[5].location = 5;
				attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[5].offset = static_cast<uint32_t>(sizeof(float) * 3);

				attributeDescriptions[6].binding = 1;
				attributeDescriptions[6].location = 6;
				attributeDescriptions[6].format = VK_FORMAT_R32_SFLOAT;
				attributeDescriptions[6].offset = static_cast<uint32_t>(sizeof(float) * 6);

				return attributeDescriptions;
			}

			bool operator==(const Vertex &other) const
			{
				return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
			}
		};

		struct KVulkanPushConstants
		{
			VkBool32 usePhong;
			uint32_t numLights;
		};

		struct vxDynamicUBO
		{
			glm::mat4 matrix;
			glm::vec4 material;
		};

		struct KLightData
		{
			glm::vec4 pos;
			glm::vec4 color;
			glm::vec4 specular;
			glm::vec4 attenuation;
		};

		struct LightUniformBufferObject
		{
			KLightData lights[KE_MAX_DYNAMIC_LIGHTS];
		};

		struct UniformBufferObject
		{
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec4 worldAmbient;
		};

		//! Command buffer fun!
		struct KVulkanCommandSettings
		{
			VkCommandPoolCreateInfo poolInfo = {};
			VkCommandBufferAllocateInfo allocInfo = {};
			VkCommandBufferBeginInfo graphicsCmdBufferInfo = {};
			VkRenderPassBeginInfo renderPassInfo = {};

			/**
			 * \brief Recorded commands go here.
			 *
			 * Commands in this function get recorded and are not updated every frame. Let
			 * Kitty take care of this unless you know what you're doing. :)
			 */
			std::function<void(VkCommandBuffer buf)> sceneStaticRenderCallback = nullptr;

			/**
			 * \brief If you need to execute custom commands while rendering, you want this.
			 *
			 * Usually the scene instance takes care of this, don't go messing about with this
			 * unless you know what you're doing.
			 *
			 * \param [in] Buffer to feed yummy commands to.
			 * \param imageIndex [in] Index of current swap chain image being processed.
			 */
			std::function<void(VkCommandBuffer *buf, uint32_t imageIndex)> sceneRenderCallback = nullptr;

			/**
			 * \brief If you need to replace the recorded render pass code, this is the place to do it.
			 *
			 * You probably don't want to set this unless you really know what you're doing. Let
			 * Kitty do this. You should set the sceneRenderCallback in most cases where you need
			 * to execute custom render commands.
			 *
			 * All parameters are filled in by Kitty, you just need to receive and use them.
			 *
			 * \param [in] cmdBuffer Contains the command buffer KVulkan is currently executing.
			 * \param [in] pipeline Contains the active pipeline.
			 * \param [in] renderPass Contains the active render pass.
			 * \param [in] frameBuffer Cotains the active frame buffer.
			 * \param [in] swapChainExtent Contains... You guessed it, the extent of the current swap chain image!
			 */
			std::function<void(VkCommandBuffer cmdBuffer,
			                   VkPipeline pipeline,
			                   VkRenderPass renderPass,
			                   VkFramebuffer frameBuffer,
			                   VkExtent2D swapChainExtent)> graphicsCmdPoolOverride = nullptr;
		};

		//! One big, happy package for passing around custom Vulkan settings.
		struct KVulkanSettings
		{
			std::vector<const char *> deviceExtensions = {};
			VkApplicationInfo appInfo = {};
			VkPhysicalDeviceFeatures requestedFeatures = {};
			VkDeviceCreateInfo devCreateInfo = {};
			VkSurfaceFormatKHR desiredSurfaceFormat = {};
			VkPresentModeKHR desiredPresentMode = {};
			VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
			VkFramebufferCreateInfo framebufferInfo = {};
			VkSamplerCreateInfo textureSamplerInfo = {};

			//! Command buffer stuffsies.
			KVulkanCommandSettings commands;

			//! Let Kitty set this for you unless you really know what you're doing.
			std::function<void()> outdatedSwapChainCallback = nullptr;
		};

		//! Struct to make modifying the huge amount of graphics pipeline settings more easy.
		struct KVulkanGraphicsSettings
		{
			VkDescriptorSetLayoutBinding vertexShaderBinding = {};
			VkDescriptorSetLayoutBinding fragmentShaderBinding = {};
			VkDescriptorSetLayoutBinding vxUniformLayoutBinding = {};
			VkDescriptorSetLayoutBinding lightsLayoutBinding = {};
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			VkViewport viewport = {};
			VkRect2D scissor = {};
			VkPipelineViewportStateCreateInfo viewportState = {};
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			VkPushConstantRange pushConstantRange = {};
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			VkPipelineDynamicStateCreateInfo dynamicState = {};
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			VkAttachmentDescription colorAttachment = {};
			VkAttachmentReference colorAttachmentRef = {};
			VkSubpassDescription subpass = {};
			VkSubpassDependency dependency = {};
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			VkAttachmentDescription depthAttachment = {};
			VkAttachmentReference depthAttachmentRef = {};
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};

			std::vector<std::string> vertexShaders = {};
			std::vector<std::string> fragmentShaders = {};
			std::vector<std::string> instanceVertexShaders = {};

			std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {};

			bool doCreateInstancingPipeline = false;
		};
	}
}

// Vertex struct hash function
namespace std {
	template<> struct hash<Kitty::Vulkan::Vertex> {
		size_t operator()(Kitty::Vulkan::Vertex const& vertex) const
		{
			size_t h1 = hash<glm::vec3>()(vertex.pos);
			size_t h2 = hash<glm::vec3>()(vertex.color);
			size_t h3 = hash<glm::vec2>()(vertex.texCoord);
			size_t h4 = hash<glm::vec3>()(vertex.normal);

			return ((((h1 ^ (h2 << 1)) >> 1) ^ h3) << 1) ^ h4;
		}
	};
}

#endif //KENGINE_KVULKANHELPER_H
