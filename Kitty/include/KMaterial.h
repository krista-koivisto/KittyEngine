/**
 * Kitty Engine
 * KMaterial.h
 *
 * General material object containing Vulkan texture and descriptors.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KTEXTURE_H
#define KENGINE_KTEXTURE_H


#include <vulkan/vulkan.h>
#include "Vulkan/KVulkanTexture.h"

namespace Kitty
{
	enum KE_TEXTURE_PROPERTY
	{
		KT_PROP_DIFFUSE,
		KT_PROP_SPECULAR, // TODO: Implement these texture maps.
		KT_PROP_AMBIENT, // Not yet in use
		KT_PROP_OPACITY // Not yet in use
	};

	enum KE_MATERIALS
	{
		KM_SIMPLE,
		KM_PHONG
	};

	struct KMaterialProperties
	{
		VkDescriptorImageInfo descriptor = {};
		Vulkan::KVulkanTexture *diffuseTexture = {};
		KE_MATERIALS material = KM_PHONG;

		float shininess = 16.0f;
		float specularStrength = 1.0f;
		float lightReception = 1.0f;
		float ambientStrength = 0.0f;
	};

	class KMaterial
	{
	private:

	public:
		KMaterial() = default;
		~KMaterial();

		void SetTextureImage(Vulkan::KVulkanTexture *texture, KE_TEXTURE_PROPERTY prop);

		VkDescriptorSet descriptorSet = {};
		KMaterialProperties properties;
	};
}


#endif //KENGINE_KTEXTURE_H
