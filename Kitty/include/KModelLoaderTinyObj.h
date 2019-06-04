/**
 * Kitty Engine
 * KModelLoaderTinyObj.h
 *
 * This class loads Waveform .obj data into a KMesh object and returns an
 * IObject connected to the mesh. It uses tinyobjloader to load the file.
 * https://github.com/syoyo/tinyobjloader
 *
 * tinyobjloader is released under the MIT license. Read included LICENSE
 * file.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KMODELLOADERTINYOBJ_H
#define KENGINE_KMODELLOADERTINYOBJ_H


#include <unordered_map>
#include "KEngine.h"
#include "KScene.h"
#include "IModelLoader.h"

namespace Kitty
{
	class KEngine;
	class KScene;

	class KModelLoaderTinyObj : public IModelLoader
	{
	private:
		KEngine *context = nullptr;
		KScene *scene = nullptr;
		Vulkan::KVulkan *vulkan = nullptr;

		std::unordered_map<std::string, KMesh*> meshCache;

		KMesh *LoadMeshData(std::string filename);

	public:
		explicit KModelLoaderTinyObj(KEngine *mainContext, KScene *mainScene, Vulkan::KVulkan *mainVulkan = nullptr);
		~KModelLoaderTinyObj() = default;

		KObject *LoadModel(std::string filename) override;
		void SetVulkanContext(Vulkan::KVulkan *vulkanContext) override;

		void ClearCache() override;
		void RemoveFromCache(KMesh *mesh) override;
	};
}


#endif //KENGINE_KMODELLOADERTINYOBJ_H
