/**
 * Kitty Engine
 * KMaterial.cpp
 *
 * General material object containing Vulkan texture and descriptors.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */


#include "include/KMaterial.h"

namespace Kitty
{
	void KMaterial::SetTextureImage(Vulkan::KVulkanTexture *texture, KE_TEXTURE_PROPERTY prop)
	{
		switch(prop)
		{
			case KT_PROP_DIFFUSE:
				properties.diffuseTexture = texture;
				properties.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				properties.descriptor.imageView = texture->textureImageView;
				properties.descriptor.sampler = texture->textureSampler;
				break;

			default:
				break;
		}
	}

	KMaterial::~KMaterial()
	{
		delete(properties.diffuseTexture);
	}
}
