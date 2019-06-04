/**
 * Kitty Engine
 * KError.h
 *
 * This file defines error codes and declarations for error-related
 * functions like the "code-to-text" function WhatWentWrong().
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KERROR_H
#define KENGINE_KERROR_H

#include <string>

namespace Kitty
{
	namespace Error
	{
		/**
		 * \brief Kitty engine general error codes.
		 */
		enum KError
		{
			KE_UNKNOWN_ERR = -1,
			KE_STOP = 0,
			KE_OK = 1,
			KE_WINDOW_CREATE_FAIL,
			KE_INIT_FAIL,
			KE_MAX_ENTITIES,
			KE_UNKNOWN_VULKAN,
			KE_VULKAN_INIT_FAIL,
			KE_VULKAN_NO_DEVICE,
			KE_VULKAN_DEVICE_FAIL,
			KE_VULKAN_SURFACE_FAIL,
			KE_VULKAN_SWAPCHAIN_FAIL,
			KE_VULKAN_IMAGEVIEW_FAIL,
			KE_VULKAN_GPIPELINE_FAIL,
			KE_VULKAN_RENDERPASS_FAIL,
			KE_VULKAN_FRAMEBUFFER_FAIL,
			KE_VULKAN_EXT_NOT_AVAILABLE,
			KE_VULKAN_VLAYER_NOT_AVAILABLE,
			KE_VULKAN_TEXTURE_SAMPLER_FAIL,
			KE_VULKAN_DEBUG_INIT_FAIL,
			KE_VULKAN_SC_PRESENT_FAIL,
			KE_VULKAN_CMDPOOL_FAIL,
			KE_VULKAN_CMDBUFFERS_FAIL,
			KE_VULKAN_SEMAPHORE_FAIL,
			KE_VULKAN_SHADER_FAIL,
			KE_VULKAN_DRAW_FAIL,
			KE_VULKAN_MEMORY_FAIL,
			KE_VULKAN_SC_OUT_OF_DATE,
			KE_VULKAN_BUFFER_TOO_SMALL,
			KE_VULKAN_BUFFER_CREATE_FAIL,
			KE_VULKAN_DESC_SET_LAYOUT_FAIL,
			KE_VULKAN_DESC_POOL_FAIL,
			KE_VULKAN_DESC_SET_FAIL,
			KE_TEXTURE_LOAD_FAIL,
			KE_TEXTURE_ALLOC_FAIL,
			KE_UNSUPPORTED_LAYOUT,
			KE_MODEL_LOAD_FAIL,
			KE_UNKNOWN_BUFFER_TYPE,
		};

		/**
		 * \brief Error code-to-text function
		 *
		 * Got an error code back from a function? Not sure what it means? You can't go
		 * wrong with WhatWentWrong!
		 *
		 * \param code KError error code (e.g. KE_INIT_FAIL or 5)
		 * \return std::string explaining the error in something resembling English.
		 */
		std::string WhatWentWrong(KError code);
	}
}


#endif //KENGINE_KERROR_H
