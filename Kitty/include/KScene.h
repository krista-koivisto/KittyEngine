/**
 * Kitty Engine
 * KScene.h
 *
 * Scene manager for the Kitty Vulkan graphics engine. Scenes contain all
 * objects within a scene to be rendered, this class handles creation and
 * presentation of everything.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KWORLD_H
#define KENGINE_KWORLD_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "KMesh.h"
#include "Vulkan/KVulkanBuffer.h"
#include "Vulkan/KVulkanDescriptorPool.h"
#include "KEngine.h"
#include "KError.h"
#include "ITextureLoader.h"
#include "KTextureLoaderSTB.h"
#include "IModelLoader.h"
#include "KModelLoaderTinyObj.h"
#include "KObject.h"
#include "KInstancedObject.h"
#include "KMaterial.h"
#include "KLight.h"

using namespace Kitty::Error;

namespace Kitty
{
	class KEngine;
	class KMaterial;
	class ITextureLoader;
	class KObject;
	class KInstancedObject;
	class KLight;
	class IModelLoader;

	class KScene
	{
	private:
		enum KE_BUFFER_TYPE
		{
			KT_BUFFER_VERTEX,
			KT_BUFFER_INDEX,
			KT_BUFFER_INSTANCE
		};

		KEngine *context = nullptr;
		Vulkan::KVulkan *vulkan = nullptr;

		Vulkan::vxDynamicUBO *vxUBO = nullptr;

		Vulkan::KVulkanBuffer *instanceBuffer = nullptr;
		Vulkan::KVulkanBuffer *vertexBuffer = nullptr;
		Vulkan::KVulkanBuffer *indexBuffer = nullptr;
		Vulkan::KVulkanBuffer *dummyInstanceBuffer = nullptr;
		Vulkan::KVulkanBuffer *dummyIndexBuffer = nullptr;
		Vulkan::KVulkanBuffer *dummyVertexBuffer = nullptr;

		Vulkan::KVulkanBuffer *lightsBuffer = {};
		Vulkan::KVulkanBuffer *uniformBuffer = {};
		Vulkan::KVulkanBuffer *vxDynamicBuffer = {};
		VkDescriptorSet lightsDescriptorSet = {};
		VkDescriptorSet uniformDescriptorSet = {};
		VkDescriptorSet vxDynamicUniformDescriptorSet = {};
		size_t dynamicAlignment = 0;

		std::vector<KInstancedObject*> instancedObjects = {};
		std::vector<KObject*> objects = {};
		std::vector<KMaterial*> materials = {};
		std::vector<KLight*> lights = {};
		KMaterial *dummyMat = {};

		VkMemoryPropertyFlags srcMemFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		VkBufferUsageFlags srcBufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags vertexMemFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkBufferUsageFlags vertexBufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VkMemoryPropertyFlags indexMemFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkBufferUsageFlags indexBufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		ITextureLoader *texLoader = nullptr;
		bool hasUserSetTextureLoader = true;

		IModelLoader *objLoader = nullptr;
		bool hasUserSetModelLoader = true;

		/**
		 * \brief Create a UBO for passing view and projection information to the vertex shader.
		 */
		void CreateUniformBuffers();

		/**
		 * \brief Create dynamic UBOs for passing object translation matrices to the shader.
		 */
		void CreateDynamicUniformBuffers();

		/**
		 * \brief Update vertex and index buffers.
		 *
		 * Copies vertex and index data to their respective buffers.
		 *
		 * \tparam T Data type.
		 * \param data Data to copy to the target buffer.
		 * \param type What type of buffer are we creating?
		 * \return Pointer to the shiny new buffer.
		 */
		template <typename T>
		Vulkan::KVulkanBuffer * CreateObjectBuffer(std::vector<T> data, KE_BUFFER_TYPE type);

		/**
		 * \brief Update dynamic uniform buffer data.
		 */
		void UpdateDynamicUniformBuffers();

		/**
		 * \brief Update object data in the dynamic object buffer.
		 *
		 * \param obj Object whose data we want to update.
		 * \param index Index of the object in the buffer.
		 */
		void UpdateDynamicObjectBuffer(IObject *obj, uint32_t index);

		/**
		 * \brief Pass information to Vulkan about what we need from our layouts.
		 */
		void PrepareDescriptorLayouts();

		/**
		 * \brief Create descriptor sets for loaded shaders.
		 */
		void InitializeDescriptorSets();

		/**
		 * \brief Default recorded render pass command callback.
		 *
		 * \param buf [in] Command buffer currently being processed by the command pool.
		 */
		void StaticRenderCallback(VkCommandBuffer buf);

		/**
		 * \brief Draw all regular objects created by the scene.
		 *
		 * \param buf [in] Command buffer currently being processed by the command pool.
		 */
		void DrawObjects(VkCommandBuffer buf);

		/**
		 * \brief Draw all instanced objects created by the scene.
		 *
		 * \param buf [in] Command buffer currently being processed by the command pool.
		 */
		void DrawInstancedObjects(VkCommandBuffer buf);

		/**
		 * \brief Default render callback function passed to Vulkan.
		 *
		 * \param buf [in] Command buffer created for this function.
		 * \param imageIndex [in] Index of current swap chain image being processed.
		 */
		void RenderCallback(VkCommandBuffer *buf, uint32_t imageIndex);

		/**
		 * \brief Delete everything created by this scene.
		 */
		void DeleteEverything();

		/**
		 * Wrapper functions for aligned memory allocation.
		 * There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this.
		 * Credits go to Sascha Willems for these. (https://github.com/SaschaWillems/)
		 */
		void* AlignedAlloc(size_t size, size_t alignment);
		void AlignedFree(void* data);
	public:
		/**
		 * \brief Create a new scene.
		 *
		 * Create a new scene or world to work in. Feel powerful.
		 *
		 * \param mainContext Main Kitty Engine context.
		 * \param vulkanContext Vulkan context.
		 * \param textureLoader [optional] Pointer to framework you want to load your textures. (e.g. Kitty::KTextureLoaderSTB)
		 * \param modelLoader [optional] Pointer to framework you want to load your models. (e.g. Kitty::KModelLoaderTinyObj)
		 * \return Pointer to the new KScene object.
		 */
		explicit KScene(KEngine *mainContext, Vulkan::KVulkan *vulkanContext,
		                ITextureLoader *textureLoader = nullptr,
		                IModelLoader *modelLoader = nullptr);

		/**
		 * \brief Destroy this scene.
		 *
		 * Destroy scene completely. Sometimes you just gotta do what you gotta do...
		 */
		~KScene();

		// TODO: Make an actual camera entity.
		glm::vec3 viewPosition = glm::vec3(2, 2, 2);
		glm::vec3 viewRotation = -viewPosition;

		/**
		 * \brief Create a new model object.
		 *
		 * Creates a new object from a model. A filename may be provided to load mesh data
		 * from into the model as well.
		 *
		 * \param filename Directory and name of the model file to load.
		 * \return Pointer to object created from the model.
		 */
		KObject *LoadModel(std::string filename);

		/**
		 * \brief Create a new instance of an object.
		 *
		 * NOTE: This is called automatically when an instance is created from a parent
		 * object, if you want to create an instance of an object call CreateInstance
		 * from the parent object.
		 *
		 * \param parent Parent object from which to create instance.
		 * \return Pointer to the newly created instance.
		 */
		KInstancedObject *AddObjectInstance(IObject *parent);

		/**
		 * \brief Create a material with a texture from an image.
		 *
		 * \param filename Directory and name of the image to load.
		 * \return Pointer to material created from the image.
		 */
		KMaterial *LoadImageTexture(std::string filename);

		/**
		 * \brief Create a material with a 2D texture from R8G8B8A8 data.
		 *
		 * Create a texture from data. The format must be R8G8B8A8, meaning the texels you
		 * provide must be in groups of four, one byte long each.
		 *
		 * \param data Data from which to create texture.
		 * \param width Width of the texture to be created.
		 * \param heigt Height of the texture to be created.
		 * \return Pointer to material created from data.
		 */
		KMaterial *Generate2DTexture(unsigned char *data, uint32_t width, uint32_t height);

		/**
		 * \brief Create a material with a square 2D texture from R8G8B8A8 data.
		 *
		 * \param data Data from which to create texture.
		 * \return Pointer to material created from data.
		 */
		KMaterial *Generate2DTexture(std::vector<unsigned char> data)
		{
			auto size = static_cast<uint32_t>(sqrt(data.size()));
			if (size % 2) throw std::runtime_error("Size of texture must be power of 2.");

			return Generate2DTexture(data.data(), size / 2, size / 2);
		};

		/**
		 * \brief Light up your world!
		 *
		 * \return Pointer to the created light object.
		 */
		KLight *CreateLight();

		/**
		 * \brief Update the scene.
		 *
		 * Updates the uniform buffer object with new view, projection and light data. You would
		 * typically call this once per frame.
		 */
		void Update();

		/**
		 * \brief Clear the scene.
		 *
		 * Remove all created objects and textures from the scene.
		 *
		 * \return KE_OK on success, error code on fail.
		 */
		KError Clear();

		/**
		 * \brief Update Vulkan with new vertex data.
		 *
		 * This function is called when an object's vertex data needs to be updated. You do not need
		 * to call this unless you manually update a mesh.
		 *
		 * \param obj Object whose data needs to be updated.
		 */
		void UpdateObject(KObject *obj);

		/**
		 * \brief Update vertex and index buffers.
		 */
		void Actualize();

		uint32_t GetMaxDynamicLights() { return KE_MAX_DYNAMIC_LIGHTS; };
	};
}


#endif //KENGINE_KWORLD_H
