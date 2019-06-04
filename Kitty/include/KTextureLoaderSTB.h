/**
 * Kitty Engine
 * KTextureLoaderSTB.h
 *
 * Loads image data into textures using the stb_image.h library.
 * https://github.com/nothings/stb
 *
 * stb_image.h library is released under the public domain.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KTEXTURELOADERSTB_H
#define KENGINE_KTEXTURELOADERSTB_H

#include "ITextureLoader.h"
#include "KEngine.h"
#include "KMaterial.h"
#include "Vulkan/KVulkan.h"
#include "KScene.h"

namespace Kitty
{
	class KEngine;
	class KScene;

	class KTextureLoaderSTB : public ITextureLoader
	{
	private:
		KEngine *context = nullptr;
		Vulkan::KVulkan *vulkan = nullptr;

	public:
		explicit KTextureLoaderSTB(KEngine *mainContext, Vulkan::KVulkan *mainVulkan = nullptr);
		~KTextureLoaderSTB() = default;

		KMaterial *LoadImage(std::string filename, KE_TEXTURE_PROPERTY prop) override;
		void SetVulkanContext(Vulkan::KVulkan *vulkanContext) override;
	};
}


#endif //KENGINE_KTEXTURELOADERSTB_H
