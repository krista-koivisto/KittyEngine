/**
 * Kitty engine Vulkan implementation
 * KVulkan.cpp
 *
 * Vulkan core implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkan.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkan::KVulkan(Window::IWindow *targetWindow, KVulkanSettings *vulkanSettings, KVulkanGraphicsSettings *vulkanGraphicsSettings)
		{
			instance = nullptr;
			window = targetWindow;
			settings = vulkanSettings;
			graphicsSettings = vulkanGraphicsSettings;

			KError ret;

			ret = Initialize();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = InitializeDevice();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = InitializeDescriptorLayouts();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = InitializeGraphics();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = InitializeSemaphores();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));
		}

		void KVulkan::DrawFrame()
		{
			uint32_t imageIndex;
			VkCommandBuffer commandBuffer;
			VkQueue graphicsQueue = device->graphicsQueue;
			VkQueue presentQueue = device->presentQueue;
			std::vector<VkCommandBuffer> cmdBuffers;

			VkResult result = vkAcquireNextImageKHR(device->device, swapChain->swapChain,
			                                        std::numeric_limits<uint64_t>::max(),
			                                        imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				if (outdatedSwapChainCallback != nullptr)
				{
					outdatedSwapChainCallback();
					return;
				}
				else
				{
					throw std::runtime_error(WhatWentWrong(KE_VULKAN_SC_OUT_OF_DATE));
				}
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_DRAW_FAIL));
			}

			VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
			VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
			VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

			VkSubmitInfo si = {};
			si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			si.pNext = nullptr;
			si.waitSemaphoreCount = 1;
			si.pWaitSemaphores = waitSemaphores;
			si.pWaitDstStageMask = waitStages;
			si.signalSemaphoreCount = 1;
			si.pSignalSemaphores = signalSemaphores;

			if (settings->commands.sceneRenderCallback != nullptr)
			{
				commandBuffer = cmdPool->InitiateCommand();
				settings->commands.sceneRenderCallback(&commandBuffer, imageIndex);
				cmdBuffers.push_back(commandBuffer);
			}

			cmdBuffers.push_back(cmdPool->commandBuffers[imageIndex]);
			si.commandBufferCount = static_cast<uint32_t >(cmdBuffers.size());
			si.pCommandBuffers = cmdBuffers.data();

			if (vkQueueSubmit(graphicsQueue, 1, &si, VK_NULL_HANDLE) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_DRAW_FAIL));
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = {swapChain->swapChain};
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;

			if (vkQueuePresentKHR(presentQueue, &presentInfo) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_DRAW_FAIL));
			}

			vkQueueWaitIdle(presentQueue);
			if (settings->commands.sceneRenderCallback != nullptr)
			{
				vkFreeCommandBuffers(device->device, cmdPool->commandPool, 1, &commandBuffer);
			}
		}

		void KVulkan::FinishDrawing()
		{
			vkDeviceWaitIdle(device->device);
		}

		void KVulkan::RecreateSwapChain()
		{
			DestroySwapChain();
			InitializeGraphics();
		}

		void KVulkan::RecreateDescriptorPool()
		{
			DestroyDescriptorPool();
			InitializeDescriptorLayouts();
		}

		void KVulkan::RecreateCommandPool()
		{
			FinishDrawing();
			DestroyCommandPools();

			KError ret =  InitializeCommandPools();
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = cmdPool->InitializeGraphicsBuffer(&settings->commands);
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));

			ret = transferCmdPool->InitializeTransferBuffer(&settings->commands);
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));
		}

		KError KVulkan::Initialize()
		{
			KError ret;

			outdatedSwapChainCallback = settings->outdatedSwapChainCallback;

			ret = InitializeInstance();
			if (ret != KE_OK) return ret;

			if (!InitDebug()) return KE_VULKAN_DEBUG_INIT_FAIL;

			if (window->CreateVulkanSurface(instance, &surface) != Window::KW_OK)
			{
				return KE_VULKAN_SURFACE_FAIL;
			}

			return KE_OK;
		}

		KError KVulkan::InitializeGraphics()
		{
			KError ret;

			ret = InitializeSwapChain();
			if (ret != KE_OK) return ret;

			ret = InitializeRenderPass();
			if (ret != KE_OK) return ret;

			ret = InitializeGraphicsPipelines();
			if (ret != KE_OK) return ret;

			ret = InitializeCommandPools();
			if (ret != KE_OK) return ret;

			ret = InitializeDepthBuffer();
			if (ret != KE_OK) return ret;

			ret = InitializeFramebuffer();
			if (ret != KE_OK) return ret;

			ret = cmdPool->InitializeGraphicsBuffer(&settings->commands);
			if (ret != KE_OK) return ret;

			ret = transferCmdPool->InitializeTransferBuffer(&settings->commands);
			if (ret != KE_OK) return ret;
		}

		KError KVulkan::InitializeInstance(VkApplicationInfo *applicationInfo)
		{
			std::vector<const char *> extensions;

			// Ask the window manager which Vulkan extensions it requires
			extensions = window->GetExtensions();
			static auto availableExts = GetAvailableExtensions();

			auto appInfo = defaults.ObtainValues(applicationInfo, &defaults.appInfo);
			if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

			// Make sure Vulkan supports the requested extensions
			if (!ValidateExtensionSupport(availableExts, extensions))
				return KE_VULKAN_EXT_NOT_AVAILABLE;

			VkInstanceCreateInfo createInfo = defaults.createInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();
			createInfo.pApplicationInfo = &appInfo;

			if (enableValidationLayers)
			{
				// Make sure our requested validation layers are supported
				static auto availableLayers = GetAvailableValidationLayers();
				if (!ValidateValidationLayerSupport(availableLayers, validationLayers))
					return KE_VULKAN_VLAYER_NOT_AVAILABLE;

				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}

			if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			{
				return KE_VULKAN_INIT_FAIL;
			}

			return KE_OK;
		}

		KError KVulkan::InitializeDevice()
		{
			device = new KVulkanDevice(this);
			return device->Initialize(&settings->requestedFeatures, &settings->devCreateInfo, &settings->deviceExtensions);
		}

		KError KVulkan::InitializeSwapChain()
		{
			swapChain = new KVulkanSwapChain(this);
			return swapChain->Initialize(&settings->desiredSurfaceFormat,
			                             &settings->desiredPresentMode,
			                             &settings->swapChainCreateInfo);
		}

		KError KVulkan::InitializeDescriptorLayouts()
		{
			descPool = new Vulkan::KVulkanDescriptorPool(this, graphicsSettings->descriptorPoolSizes);

			// Initialize layouts and bindings for the descriptors
			if (!descPool->InitializeBinding(&graphicsSettings->vertexShaderBinding, &vertexDescriptorLayout))
			{
				return KE_VULKAN_DESC_SET_LAYOUT_FAIL;
			}

			if (!descPool->InitializeBinding(&graphicsSettings->fragmentShaderBinding, &fragmentDescriptorLayout))
			{
				return KE_VULKAN_DESC_SET_LAYOUT_FAIL;
			}

			if (!descPool->InitializeBinding(&graphicsSettings->vxUniformLayoutBinding, &vxUniformBufferDescriptorLayout))
			{
				return KE_VULKAN_DESC_SET_LAYOUT_FAIL;
			}

			if (!descPool->InitializeBinding(&graphicsSettings->lightsLayoutBinding, &lightsDescriptorLayout))
			{
				return KE_VULKAN_DESC_SET_LAYOUT_FAIL;
			}

			return KE_OK;
		}

		KError KVulkan::InitializeGraphicsPipelines()
		{
			std::vector<VkDescriptorSetLayout> setLayouts = { vertexDescriptorLayout,
			                                                  fragmentDescriptorLayout,
			                                                  vxUniformBufferDescriptorLayout,
			                                                  lightsDescriptorLayout };

			// General pipeline
			graphicsSettings->pipelineLayoutInfo.pSetLayouts = setLayouts.data();
			graphicsSettings->pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
			graphicsSettings->pipelineInfo.renderPass = mainRenderPass->renderPass;

			VkVertexInputBindingDescription bindDesc = Vertex::getBindingDescription();
			std::array<VkVertexInputAttributeDescription, 4> attribDesc = Vertex::getAttributeDescriptions();
			graphicsSettings->vertexInputInfo.pVertexBindingDescriptions = &bindDesc;
			graphicsSettings->vertexInputInfo.vertexBindingDescriptionCount = 1;
			graphicsSettings->vertexInputInfo.pVertexAttributeDescriptions = attribDesc.data();
			graphicsSettings->vertexInputInfo.vertexAttributeDescriptionCount = attribDesc.size();

			mainPipeline = new KVulkanGraphicsPipeline(this);
			KError ret = mainPipeline->Initialize(graphicsSettings);
			if (ret != KE_OK) return ret;

			// Instance pipeline
			if (graphicsSettings->doCreateInstancingPipeline)
			{
				auto instanceSettings = *graphicsSettings;
				std::array<VkVertexInputBindingDescription, 2> inBindDesc = Vertex::getInstanceBindingDescription();
				std::array<VkVertexInputAttributeDescription, 7> inAttribDesc = Vertex::getInstanceAttributeDescriptions();
				instanceSettings.vertexInputInfo.pVertexBindingDescriptions = inBindDesc.data();
				instanceSettings.vertexInputInfo.vertexBindingDescriptionCount = inBindDesc.size();
				instanceSettings.vertexInputInfo.pVertexAttributeDescriptions = inAttribDesc.data();
				instanceSettings.vertexInputInfo.vertexAttributeDescriptionCount = inAttribDesc.size();
				instanceSettings.vertexShaders = instanceSettings.instanceVertexShaders;

				instancePipeline = new KVulkanGraphicsPipeline(this);
				ret = instancePipeline->Initialize(&instanceSettings);
			}

			return ret;
		}

		KError KVulkan::InitializeRenderPass()
		{
			mainRenderPass = new KVulkanRenderPass(this);

			return KE_OK;
		}

		KError KVulkan::InitializeFramebuffer()
		{
			frameBuffer = new KVulkanFramebuffer(this);
			return frameBuffer->Initialize(&settings->framebufferInfo);
		}

		KError KVulkan::InitializeCommandPools()
		{
			settings->commands.poolInfo.queueFamilyIndex = device->features.graphicsFamily;
			cmdPool = new KVulkanCommandPool(this, &settings->commands);
			settings->commands.poolInfo.queueFamilyIndex = device->features.transferFamily;
			transferCmdPool = new KVulkanCommandPool(this, &settings->commands);

			return KE_OK;
		}

		KError KVulkan::InitializeSemaphores()
		{
			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			    vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
			{
				return KE_VULKAN_SEMAPHORE_FAIL;
			}

			return KE_OK;
		}

		KError KVulkan::InitializeDepthBuffer()
		{
			auto depthFormat = device->features.depthFormat;
			depthImage = new KVulkanImage(this, swapChain->swapChainExtent.width,
			                                    swapChain->swapChainExtent.height,
			                                    depthFormat, VK_IMAGE_TILING_OPTIMAL,
			                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			depthImageView = new Vulkan::KVulkanImageView(this);
			auto createInfo = defaults.imageViewCreateInfo;
			createInfo.format = depthFormat;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			depthImageView->Initialize(createInfo, depthImage->image);
			depthImage->TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
			                                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

			return KE_OK;
		}

		std::vector<VkExtensionProperties> KVulkan::GetAvailableExtensions()
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			return extensions;
		}

		std::vector<VkLayerProperties> KVulkan::GetAvailableValidationLayers()
		{
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			return availableLayers;
		}

		bool KVulkan::ValidateExtensionSupport(std::vector<VkExtensionProperties> available,
		                                       std::vector<const char *> requested)
		{
			for (auto request : requested)
			{
				std::string req(request);
				auto it = std::find_if(available.begin(), available.end(),
				                       [&r = req](const VkExtensionProperties &a) -> bool
				                       {
					                       return !r.compare(a.extensionName);
				                       });

				if (it == available.end()) return false;
			}

			return true;
		}

		bool KVulkan::ValidateValidationLayerSupport(std::vector<VkLayerProperties> available,
		                                             std::vector<const char *> requested)
		{
			for (auto request : requested)
			{
				std::string req(request);
				auto it = std::find_if(available.begin(), available.end(),
				                       [&r = req](const VkLayerProperties &a) -> bool
				                       {
					                       return !r.compare(a.layerName);
				                       });

				if (it == available.end()) return false;
			}

			return true;
		}

		bool KVulkan::InitDebug(VkDebugReportCallbackCreateInfoEXT *debugInfo)
		{
			if (!enableValidationLayers) return true;

			VkDebugReportCallbackCreateInfoEXT info = defaults.ObtainValues(debugInfo, &defaults.debugInfo);
			info.pfnCallback = VKDebugCallback;

			auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance,
			                                                                       "vkCreateDebugReportCallbackEXT");

			if (func != nullptr)
			{
				if (func(instance, &info, nullptr, &callback) != VK_SUCCESS)
				{
					return false;
				}
			}

			return true;
		}

		void KVulkan::DestroyDebug()
		{
			if (!enableValidationLayers) return;

			auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance,
			                                                                        "vkDestroyDebugReportCallbackEXT");

			if (func != nullptr)
			{
				func(instance, callback, nullptr);
			}
		}

		void KVulkan::DestroyDescriptorPool()
		{
			delete(descPool);

			vkDestroyDescriptorSetLayout(device->device, vertexDescriptorLayout, nullptr);
			vkDestroyDescriptorSetLayout(device->device, fragmentDescriptorLayout, nullptr);
			vkDestroyDescriptorSetLayout(device->device, vxUniformBufferDescriptorLayout, nullptr);
			vkDestroyDescriptorSetLayout(device->device, lightsDescriptorLayout, nullptr);
			vertexDescriptorLayout = {};
			fragmentDescriptorLayout = {};
			vxUniformBufferDescriptorLayout = {};
			lightsDescriptorLayout = {};
		}

		void KVulkan::DestroyCommandPools()
		{
			delete(transferCmdPool);
			delete(cmdPool);
		}

		void KVulkan::DestroySwapChain()
		{
			DestroyCommandPools();
			delete(frameBuffer);
			delete(depthImage);
			delete(depthImageView);
			delete(mainRenderPass);
			delete(mainPipeline);
			delete(instancePipeline);
			delete(swapChain);
		}

		KVulkan::~KVulkan()
		{
			vkDestroySemaphore(device->device, renderFinishedSemaphore, nullptr);
			vkDestroySemaphore(device->device, imageAvailableSemaphore, nullptr);

			DestroyDescriptorPool();
			DestroySwapChain();
			delete (device);

			vkDestroySurfaceKHR(instance, surface, nullptr);
			DestroyDebug();
			vkDestroyInstance(instance, nullptr);
		}
	}
}
