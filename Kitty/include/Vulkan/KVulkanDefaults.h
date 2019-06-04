/**
 * Kitty engine Vulkan implementation
 * KVulkanDefaults.h
 *
 * Default values for Vulkan structures are kept here for easy
 * initialization and access to changing single values when needed.
 * Also, some functions to make interacting with the defaults
 * easier.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANDEFAULTS_H
#define KENGINE_KVULKANDEFAULTS_H

#include <vulkan/vulkan.h>
#include <vector>
#include "KVulkanHelpers.h"

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkanDefaults
		{
		public:
			std::array<VkClearValue, 2> clear = {};
			VkVertexInputBindingDescription bindingDescription = {};
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			// Vulkan settings
			KVulkanSettings vulkanSettings = {};
			std::vector<const char *> deviceExtensions = {};
			VkApplicationInfo appInfo = {};
			VkInstanceCreateInfo createInfo = {};
			VkDebugReportCallbackCreateInfoEXT debugInfo = {};
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			VkPhysicalDeviceFeatures deviceFeatures = {};
			VkDeviceCreateInfo deviceCreateInfo = {};
			VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
			VkFramebufferCreateInfo framebufferInfo = {};
			VkSamplerCreateInfo textureSamplerInfo = {};

			// Command settings
			VkCommandPoolCreateInfo poolInfo = {};
			VkCommandBufferAllocateInfo bufferAllocationInfo = {};
			VkCommandBufferBeginInfo graphicsCmdBufferInfo = {};
			VkCommandBufferBeginInfo transferCmdBufferInfo = {};
			VkRenderPassBeginInfo renderPassInfo = {};

			VkImageViewCreateInfo imageViewCreateInfo = {};

			// Let's prefer the pretty-to-humans sRGB color space if the user doesn't supply one.
			VkSurfaceFormatKHR desiredSurfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

			// Enable triple buffering by default.
			VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

			// Just a few structs for the graphics pipeline alone...
			KVulkanGraphicsSettings graphicsPipelineInfo = {};
			VkDescriptorSetLayoutBinding vertexLayoutBinding = {};
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
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			VkAttachmentDescription colorAttachment = {};
			VkAttachmentReference colorAttachmentRef = {};
			VkSubpassDescription subpass = {};
			VkSubpassDependency dependency = {};
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			VkAttachmentDescription depthAttachment = {};
			VkAttachmentReference depthAttachmentRef = {};
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {};

			/**
			 * \brief Constructor. Sets default values for structures.
			 *
			 * This is going to get long, Vulkan likes giving us options. But I guess
			 * you can't really argue with that! :)
			 */
			KVulkanDefaults()
			{
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = desiredSurfaceFormat.format;
				imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;

				bindingDescription = Vertex::getBindingDescription();
				attributeDescriptions = Vertex::getAttributeDescriptions();

				VulkanSettings();
				GraphicsPipelineSettings();
			};

			~KVulkanDefaults() = default;

			/**
			 * \brief Get passed struct values if any, defaults otherwise.
			 *
			 * If values were passed, this function returns them. Otherwise it will return the defaults instead.
			 *
			 * \param passedPtr Struct pointer passed to function. (May point to struct data or be nullptr.)
			 * \param defaultsPtr Pointer to the default values for that struct.
			 * \return The data (not a pointer!) contained in the used struct.
			 */
			template<typename T>
			auto ObtainValues(T *passedPtr, T *defaultsPtr)
			{
				if (passedPtr == nullptr)
				{
					return *defaultsPtr;
				}
				else
				{
					return *passedPtr;
				}
			}

		private:
			void VulkanSettings()
			{
				deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

				deviceFeatures.samplerAnisotropy = VK_TRUE;

				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pEngineName = "Kitty Engine";
				appInfo.pApplicationName = "Kitty App";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VK_API_VERSION_1_0;

				createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

				debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
				debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
				                  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

				deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				deviceCreateInfo.enabledExtensionCount = 0;
				deviceCreateInfo.enabledLayerCount = 0;

				swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapChainCreateInfo.imageArrayLayers = 1;
				swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				swapChainCreateInfo.clipped = VK_TRUE;

				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.queueFamilyIndex = UINT32_MAX;
				poolInfo.flags = 0;

				bufferAllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				bufferAllocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.layers = 1;

				graphicsCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				graphicsCmdBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

				transferCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				transferCmdBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

				clear[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
				clear[1].depthStencil = {1.0f, 0};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderArea.offset = {0, 0};
				renderPassInfo.clearValueCount = static_cast<uint32_t>(clear.size());
				renderPassInfo.pClearValues = clear.data();

				textureSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				textureSamplerInfo.magFilter = VK_FILTER_LINEAR;
				textureSamplerInfo.minFilter = VK_FILTER_LINEAR;
				textureSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				textureSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				textureSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				textureSamplerInfo.anisotropyEnable = VK_TRUE;
				textureSamplerInfo.maxAnisotropy = 16;
				textureSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				textureSamplerInfo.unnormalizedCoordinates = VK_FALSE;
				textureSamplerInfo.compareEnable = VK_FALSE;
				textureSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
				textureSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

				vulkanSettings.deviceExtensions = deviceExtensions;
				vulkanSettings.appInfo = appInfo;
				vulkanSettings.requestedFeatures = deviceFeatures;
				vulkanSettings.devCreateInfo = deviceCreateInfo;
				vulkanSettings.desiredSurfaceFormat = desiredSurfaceFormat;
				vulkanSettings.desiredPresentMode = desiredPresentMode;
				vulkanSettings.swapChainCreateInfo = swapChainCreateInfo;
				vulkanSettings.framebufferInfo = framebufferInfo;
				vulkanSettings.textureSamplerInfo = textureSamplerInfo;

				vulkanSettings.outdatedSwapChainCallback = nullptr;

				// Command settings
				vulkanSettings.commands.poolInfo = poolInfo;
				vulkanSettings.commands.allocInfo = bufferAllocationInfo;
				vulkanSettings.commands.graphicsCmdBufferInfo = graphicsCmdBufferInfo;
				vulkanSettings.commands.renderPassInfo = renderPassInfo;
				vulkanSettings.commands.graphicsCmdPoolOverride = nullptr;
			}

			void GraphicsPipelineSettings()
			{
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

				inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssembly.primitiveRestartEnable = VK_FALSE;

				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				scissor.offset = {0, 0};
				scissor.extent = {0, 0};

				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;

				rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizer.depthClampEnable = VK_FALSE;
				rasterizer.rasterizerDiscardEnable = VK_FALSE;
				rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizer.lineWidth = 1.0f;
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizer.depthBiasEnable = VK_FALSE;

				multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling.sampleShadingEnable = VK_FALSE;
				multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

				colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
				                                      VK_COLOR_COMPONENT_G_BIT |
				                                      VK_COLOR_COMPONENT_B_BIT |
				                                      VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachment.blendEnable = VK_FALSE;

				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = 1;
				colorBlending.blendConstants[0] = 0.0f;
				colorBlending.blendConstants[1] = 0.0f;
				colorBlending.blendConstants[2] = 0.0f;
				colorBlending.blendConstants[3] = 0.0f;

				vertexLayoutBinding.binding = 0;
				vertexLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				vertexLayoutBinding.descriptorCount = 1;
				vertexLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

				lightsLayoutBinding.binding = 0;
				lightsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				lightsLayoutBinding.descriptorCount = 1;
				lightsLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

				fragmentShaderBinding.binding = 0;
				fragmentShaderBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				fragmentShaderBinding.descriptorCount = 1;
				fragmentShaderBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

				vxUniformLayoutBinding.binding = 0;
				vxUniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				vxUniformLayoutBinding.descriptorCount = 1;
				vxUniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 1;
				pipelineLayoutInfo.pushConstantRangeCount = 0;

				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;

				pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				pushConstantRange.offset = 0;
				pushConstantRange.size = 0;

				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;

				renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCreateInfo.attachmentCount = 1;
				renderPassCreateInfo.subpassCount = 1;

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				depthAttachmentRef.attachment = 1;
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencil.depthTestEnable = VK_TRUE;
				depthStencil.depthWriteEnable = VK_TRUE;
				depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
				depthStencil.depthBoundsTestEnable = VK_FALSE;
				depthStencil.stencilTestEnable = VK_FALSE;

				VkDescriptorPoolSize size = {};
				size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				size.descriptorCount = 1;
				descriptorPoolSizes.push_back(size);

				graphicsPipelineInfo.layoutInfo = layoutInfo;
				graphicsPipelineInfo.vertexShaderBinding = vertexLayoutBinding;
				graphicsPipelineInfo.fragmentShaderBinding = fragmentShaderBinding;
				graphicsPipelineInfo.vxUniformLayoutBinding = vxUniformLayoutBinding;
				graphicsPipelineInfo.lightsLayoutBinding = lightsLayoutBinding;
				graphicsPipelineInfo.pipelineInfo = pipelineInfo;
				graphicsPipelineInfo.pushConstantRange = pushConstantRange;
				graphicsPipelineInfo.pipelineLayoutInfo = pipelineLayoutInfo;
				graphicsPipelineInfo.colorBlending = colorBlending;
				graphicsPipelineInfo.colorBlendAttachment = colorBlendAttachment;
				graphicsPipelineInfo.multisampling = multisampling;
				graphicsPipelineInfo.rasterizer = rasterizer;
				graphicsPipelineInfo.viewportState = viewportState;
				graphicsPipelineInfo.scissor = scissor;
				graphicsPipelineInfo.inputAssembly = inputAssembly;
				graphicsPipelineInfo.viewport = viewport;
				graphicsPipelineInfo.vertexInputInfo = vertexInputInfo;
				graphicsPipelineInfo.colorAttachment = colorAttachment;
				graphicsPipelineInfo.colorAttachmentRef = colorAttachmentRef;
				graphicsPipelineInfo.subpass = subpass;
				graphicsPipelineInfo.renderPassCreateInfo = renderPassCreateInfo;
				graphicsPipelineInfo.depthAttachment = depthAttachment;
				graphicsPipelineInfo.depthAttachmentRef = depthAttachmentRef;
				graphicsPipelineInfo.dependency = dependency;
				graphicsPipelineInfo.depthStencil = depthStencil;

				graphicsPipelineInfo.vertexShaders = { "Shaders/Compiled/uber.vert.spv" };
				graphicsPipelineInfo.fragmentShaders = { "Shaders/Compiled/uber.frag.spv" };
				graphicsPipelineInfo.instanceVertexShaders = { "Shaders/Compiled/instance.vert.spv" };

				graphicsPipelineInfo.descriptorPoolSizes = descriptorPoolSizes;
			}
		};
	}
}

#endif //KENGINE_KVULKANDEFAULTS_H
