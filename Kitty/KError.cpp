/**
 * Kitty Engine
 * KError.cpp
 *
 * This file implements error handling related functions, like the
 * "code-to-text" function WhatWentWrong().
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KError.h"

namespace Kitty
{
	namespace Error
	{
		std::string WhatWentWrong(KError code)
		{
			std::string unknownError = "I don't like it. Brain, I don't like it ONE BIT. ... Not. One. Bit. :<";

			switch(code)
			{
				case KE_STOP: return "Just letting you know the main loop stopped, someone probably closed the rendering window?";
				case KE_OK: return "Puurrrr! :3 (Nothing is wrong, but thanks for asking!)";
				case KE_WINDOW_CREATE_FAIL: return "Failed to create window!";
				case KE_INIT_FAIL: return "I failed to initialize. :(";
				case KE_MAX_ENTITIES: return "You have reached the maximum number of entities allowed for this type!";
				case KE_VULKAN_INIT_FAIL: return "Vulkan failed to initialize. :(";
				case KE_VULKAN_NO_DEVICE: return "Failed to find a suitable GPU device!";
				case KE_VULKAN_DEVICE_FAIL: return "Failed to create a logical Vulkan device! (Oh, the irony...)";
				case KE_VULKAN_SURFACE_FAIL: return "Failed to create a Vulkan window surface!";
				case KE_VULKAN_SWAPCHAIN_FAIL: return "Failed to create a swap chain!";
				case KE_VULKAN_IMAGEVIEW_FAIL: return "Failed to create Vulkan image view!";
				case KE_VULKAN_GPIPELINE_FAIL: return "Failed to create Vulkan graphics pipeline! (This could indicate an error with your shaders.)";
				case KE_VULKAN_RENDERPASS_FAIL: return "Failed to create Vulkan render pass!";
				case KE_VULKAN_FRAMEBUFFER_FAIL: return "Failed to create Vulkan frame buffers!";
				case KE_VULKAN_EXT_NOT_AVAILABLE: return "The window manager requested an unavailable Vulkan extension, unable to continue!";
				case KE_VULKAN_VLAYER_NOT_AVAILABLE: return "The requested validation layers are not available in your Vulkan driver, unable to continue!";
				case KE_VULKAN_TEXTURE_SAMPLER_FAIL: return "Failed to create Vulkan texture sampler!";
				case KE_VULKAN_DEBUG_INIT_FAIL: return "Failed to initialize Vulkan debugging, unable to continue!";
				case KE_VULKAN_SC_PRESENT_FAIL: return "Failed to present Vulkan swap chain!";
				case KE_VULKAN_CMDPOOL_FAIL: return "Failed to create Vulkan command pool!";
				case KE_VULKAN_CMDBUFFERS_FAIL: return "Failed to create Vulkan command buffers!";
				case KE_VULKAN_SEMAPHORE_FAIL: return "Failed to create Vulkan semaphore! Sorry! :(";
				case KE_VULKAN_SHADER_FAIL: return "Failed to load shader module. Unable to continue!";
				case KE_VULKAN_DRAW_FAIL: return "Vulkan failed to draw, but I have no idea what happened. Sorry! :(";
				case KE_VULKAN_MEMORY_FAIL: return "Failed to find suitable memory type when attempting to create Vulkan buffer!";
				case KE_VULKAN_SC_OUT_OF_DATE: return "Vulkan's swap chain is out of date and no callback is available to handle it. I can't deal with this!";
				case KE_VULKAN_BUFFER_TOO_SMALL: return "Poured a gallon of milk into a 16 oz glass. Now I'm sad. (Can't copy a larger buffer into a smaller one, that won't work!)";
				case KE_VULKAN_BUFFER_CREATE_FAIL: return "Failed to create buffer!";
				case KE_VULKAN_DESC_SET_LAYOUT_FAIL: return "Failed to create descriptor set layout!";
				case KE_VULKAN_DESC_POOL_FAIL: return "Failed to create descriptor pool!";
				case KE_VULKAN_DESC_SET_FAIL: return "Failed to create descriptor set!";
				case KE_TEXTURE_LOAD_FAIL: return "Failed to load texture image from file!";
				case KE_TEXTURE_ALLOC_FAIL: return "Failed to allocate image memory!";
				case KE_UNSUPPORTED_LAYOUT: return "Unsupported layout transition when loading image for texture!";
				case KE_MODEL_LOAD_FAIL: return "Failed to load object model!";
				case KE_UNKNOWN_BUFFER_TYPE: return "Can't create buffer; unknown buffer type!";

				case KE_UNKNOWN_VULKAN:
				case KE_UNKNOWN_ERR:
				default:
					return unknownError;
			}
		}
	}
}
