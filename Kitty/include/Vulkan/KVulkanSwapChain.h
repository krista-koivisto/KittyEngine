/**
 * Kitty engine Vulkan implementation
 * KVulkanSwapChain.h
 *
 * Vulkan swap chain implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANSWAPCHAIN_H
#define KENGINE_KVULKANSWAPCHAIN_H

#include "KVulkan.h"
#include "KVulkanDefaults.h"
#include "KVulkanImageView.h"
#include "../KError.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;
		class KVulkanImageView;

		class KVulkanSwapChain
		{
		private:
			KVulkan *context;
			KVulkanDefaults defaults;

		public:
			explicit KVulkanSwapChain(KVulkan *mainContext) : context(mainContext) {}
			~KVulkanSwapChain();

			VkSwapchainKHR swapChain = VK_NULL_HANDLE;
			std::vector<VkImage> swapChainImages = {};
			std::vector<KVulkanImageView *> swapChainImageViews;
			VkFormat swapChainImageFormat = {};
			VkExtent2D swapChainExtent = {};

			struct SwapChainSupportDetails
			{
				VkSurfaceCapabilitiesKHR capabilities = {};
				std::vector<VkSurfaceFormatKHR> formats = {};
				std::vector<VkPresentModeKHR> presentModes = {};
			};

			SwapChainSupportDetails supportDetails;

			/**
			 * \brief Initialize Swap Chain instance.
			 *
			 * Initializes the swap chain instance. Custom swap chain creation info may
			 * be passed, as well as requests for desired formats and present modes. Requests
			 * are not guaranteed to be fulfilled, they will only be granted if the device is
			 * capable of it. If the device is incapable then the swap chain initializer will
			 * pick another option.
			 *
			 * \param desiredFormat [optional] Request a specific surface format.
			 * \param desiredPresentMode [optional] Request a specific present mode.
			 * \param swapChainCreateInfo Custom swap chain creation info.
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(VkSurfaceFormatKHR *desiredFormat = nullptr,
			                  VkPresentModeKHR *desiredPresentMode = nullptr,
			                  VkSwapchainCreateInfoKHR *swapChainCreateInfo = nullptr);

			/**
			 * \brief Verify adequacy of swap chain.
			 *
			 * Verifies that the swap chain capabilities and formats are adequate for our
			 * application.
			 *
			 * \param swapChainDetails Swap chain details obtained from a KVulkanSwapChain instance.
			 * \return true if swap chain details are adequate, otherwise false.
			 */
			bool IsSwapChainAdequate(KVulkanSwapChain::SwapChainSupportDetails swapChainDetails);

			/**
			 * \brief Retrieve swap chain support details.
			 *
			 * Get details on supported swap chain features from the physical device.
			 *
			 * \param physicalDevice Vulkan physical device. Yup.
			 * \return Struct containing supported swap chain details.
			 */
			SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice);

			/**
			 * \brief Set the swap surface format to use.
			 *
			 * If the user has not supplied a preferred surface format the function will try to
			 * pick the best one available. If the user's desired format is not available the
			 * function will just pick the first suggestion off the available formats list instead.
			 *
			 * \param availableFormats std::vector containing list of available formats.
			 * \param desiredFormat [optional] Ask nicely for the format of your liking.
			 * \return The chosen format.
			 */
			VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats,
			                                           VkSurfaceFormatKHR *desiredFormat = nullptr);

			/**
			 * \brief Set the best swap present mode to use.
			 *
			 * If the user has not supplied a preferred present mode, the function will try to pick
			 * the best one available. If the user's desired mode isn't available, then the function
			 * will just try to pick the first suggestion off the supplied list instead.
			 *
			 * \param availablePresentModes std::vector containing list of available present modes.
			 * \param desiredPresentMode [optional] Ask nicely for the mode of your liking.
			 * \return The chosen swap present mode.
			 */
			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes,
			                                       VkPresentModeKHR *desiredPresentMode = nullptr);

			/**
			 * \brief Set the resolution of the swap chain image.
			 *
			 * Set the extent, or the resolution, of the image being rendered to the swap chain.
			 *
			 * \param capabilities Capabilities returned from QuerySwapChainSupport.
			 * \return VkExtend2D containing the resolution at which to render the swap image.
			 */
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		};
	}
}


#endif //KENGINE_KVULKANSWAPCHAIN_H
