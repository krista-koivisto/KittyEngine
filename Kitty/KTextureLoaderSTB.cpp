/**
 * Kitty Engine
 * KTextureLoaderSTB.cpp
 *
 * Loads image data into textures using the stb_image.h library.
 * https://github.com/nothings/stb
 *
 * stb_image.h library is released under the public domain.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KTextureLoaderSTB.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

namespace Kitty
{
	KTextureLoaderSTB::KTextureLoaderSTB(KEngine *mainContext, Vulkan::KVulkan *mainVulkan)
	{
		context = mainContext;
		vulkan = mainVulkan;
	}

	KMaterial *KTextureLoaderSTB::LoadImage(std::string filename, KE_TEXTURE_PROPERTY prop)
	{
		auto tex = new KMaterial();

		// If no file was provided, just set a dummy texture
		if (filename.empty())
		{
			auto vulkanTexture = new Vulkan::KVulkanTexture(vulkan, &context->settings);
			tex->SetTextureImage(vulkanTexture, prop);

			return tex;
		}

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		auto vulkanTexture = new Vulkan::KVulkanTexture(vulkan, &context->settings);
		vulkanTexture->SetImage2D_8R8G8B8A(pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		tex->SetTextureImage(vulkanTexture, prop);

		stbi_image_free(pixels);

		return tex;
	}

	void KTextureLoaderSTB::SetVulkanContext(Vulkan::KVulkan *vulkanContext)
	{
		vulkan = vulkanContext;
	}
}
