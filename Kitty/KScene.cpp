/**
 * Kitty Engine
 * KScene.cpp
 *
 * Scene manager for the Kitty Vulkan graphics engine. Scenes contain all
 * objects within a scene to be rendered, this class handles creation and
 * presentation of everything.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KScene.h"

namespace Kitty
{
	KScene::KScene(KEngine *mainContext, Vulkan::KVulkan *vulkanContext, ITextureLoader *textureLoader, IModelLoader *modelLoader)
	{
		context = mainContext;
		vulkan = vulkanContext;

		CreateUniformBuffers();

		if (textureLoader == nullptr)
		{
			textureLoader = new KTextureLoaderSTB(context, vulkan);
			hasUserSetTextureLoader = false;
		}
		else
		{
			textureLoader->SetVulkanContext(vulkanContext);
		}

		texLoader = textureLoader;

		if (modelLoader == nullptr)
		{
			modelLoader = new KModelLoaderTinyObj(context, this, vulkan);
			hasUserSetTextureLoader = false;
		}
		else
		{
			modelLoader->SetVulkanContext(vulkanContext);
		}

		objLoader = modelLoader;

		context->settings.commands.sceneStaticRenderCallback = [this](VkCommandBuffer buf) {
			StaticRenderCallback(buf);
		};

		context->settings.commands.sceneRenderCallback = [this](VkCommandBuffer *buf, uint32_t ii)
		{
			RenderCallback(buf, ii);
		};

		dummyInstanceBuffer = new Vulkan::KVulkanBuffer(vulkan, 1, vertexBufferFlags, vertexMemFlags);
		dummyVertexBuffer = new Vulkan::KVulkanBuffer(vulkan, 1, vertexBufferFlags, vertexMemFlags);
		dummyIndexBuffer = new Vulkan::KVulkanBuffer(vulkan, 1, indexBufferFlags, indexMemFlags);
		dummyMat = LoadImageTexture("");
	}

	KError KScene::Clear()
	{
		DeleteEverything();

		Actualize();

		return KE_OK;
	}

	KObject *KScene::LoadModel(std::string filename)
	{
		auto obj = objLoader->LoadModel(std::move(filename));
		objects.push_back(obj);

		obj->SetMaterial(dummyMat);

		return obj;
	}

	KInstancedObject *KScene::AddObjectInstance(IObject *parent)
	{
		auto obj = new KInstancedObject(parent);
		instancedObjects.push_back(obj);

		return obj;
	}

	KMaterial *KScene::LoadImageTexture(std::string filename)
	{
		auto mat = texLoader->LoadImage(std::move(filename), KT_PROP_DIFFUSE);
		materials.push_back(mat);

		return mat;
	}

	KMaterial *KScene::Generate2DTexture(unsigned char *data, const uint32_t width, const uint32_t height)
	{
		auto tex = new Vulkan::KVulkanTexture(vulkan, &context->settings);
		auto mat = new KMaterial();
		tex->SetImage2D_8R8G8B8A(data, width, height);
		mat->SetTextureImage(tex, KT_PROP_DIFFUSE);
		materials.push_back(mat);

		return mat;
	}

	KLight *KScene::CreateLight()
	{
		if (lights.size() >= KE_MAX_DYNAMIC_LIGHTS)
		{
			std::cerr << "There are no more available dynamic light sources. The maximum available is "
			          << KE_MAX_DYNAMIC_LIGHTS << "! Returning last light in queue!" << std::endl;

			return lights[lights.size() -1];
		}

		auto light = new KLight(this);
		lights.push_back(light);

		return light;
	}

	void KScene::Actualize()
	{
		std::vector<Vulkan::Vertex> vx;
		std::vector<uint32_t> ix;

		uint32_t offset = 0;

		// Load object vertex and index data
		for (uint32_t i = 0; i < objects.size(); ++i)
		{
			objects[i]->GetMesh()->SetBufferOffset(offset);
			objects[i]->SetIndex(i);

			for (auto vert : objects[i]->GetMesh()->vertices)
			{
				vx.push_back(vert);
			}

			for (auto index : objects[i]->GetMesh()->indices)
			{
				ix.push_back(index);
			}

			offset += objects[i]->GetMesh()->vertices.size();
		}


		if (vertexBuffer != nullptr && vertexBuffer != dummyVertexBuffer) delete (vertexBuffer);
		if (indexBuffer != nullptr && indexBuffer != dummyIndexBuffer) delete (indexBuffer);
		vertexBuffer = CreateObjectBuffer(vx, KT_BUFFER_VERTEX);
		indexBuffer = CreateObjectBuffer(ix, KT_BUFFER_INDEX);

		// Instanced object data
		std::vector<Vulkan::InstanceData> inst;

		for (auto &object : instancedObjects)
		{
			inst.push_back(object->GetInstanceData());
		}

		if (instanceBuffer != nullptr && instanceBuffer != dummyInstanceBuffer) delete (instanceBuffer);
		instanceBuffer = CreateObjectBuffer(inst, KT_BUFFER_INSTANCE);


		CreateDynamicUniformBuffers();

		PrepareDescriptorLayouts();
		vulkan->RecreateDescriptorPool();
		InitializeDescriptorSets();

		if (!instancedObjects.empty()) vulkan->graphicsSettings->doCreateInstancingPipeline = true;
		vulkan->RecreateSwapChain();
		vulkan->RecreateCommandPool();

		uniformBuffer->Map();
		lightsBuffer->Map();
	}

	void KScene::PrepareDescriptorLayouts()
	{
		vulkan->graphicsSettings->descriptorPoolSizes.clear();
		VkDescriptorPoolSize size = {};

		// Uniform buffer
		size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		size.descriptorCount = 1;
		vulkan->graphicsSettings->descriptorPoolSizes.push_back(size);

		// Lights buffer
		size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		size.descriptorCount = 1;
		vulkan->graphicsSettings->descriptorPoolSizes.push_back(size);

		// Dynamic uniform buffers for the vertex shader
		size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		size.descriptorCount = 1;
		vulkan->graphicsSettings->descriptorPoolSizes.push_back(size);

		// Image sampler for materials
		size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		size.descriptorCount = static_cast<uint32_t>(materials.size());
		vulkan->graphicsSettings->descriptorPoolSizes.push_back(size);
	}

	void KScene::InitializeDescriptorSets()
	{
		// Material descriptor sets
		for (auto &material : materials)
		{
			vulkan->descPool->AllocateDescriptor(&vulkan->fragmentDescriptorLayout,
			                             &material->descriptorSet,
			                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			                             &material->properties.descriptor, nullptr, 0, 1);
		}

		// Vertex descriptor set
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffer->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffer->size;

		vulkan->descPool->AllocateDescriptor(&vulkan->vertexDescriptorLayout,
		                             &uniformDescriptorSet,
		                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		                             nullptr, &bufferInfo, 0, 1);

		// Lights descriptor set
		VkDescriptorBufferInfo lightsInfo = {};
		lightsInfo.buffer = lightsBuffer->buffer;
		lightsInfo.offset = 0;
		lightsInfo.range = lightsBuffer->size;

		vulkan->descPool->AllocateDescriptor(&vulkan->lightsDescriptorLayout,
		                                     &lightsDescriptorSet,
		                                     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		                                     nullptr, &lightsInfo, 0, 1);

		// Dynamic vertex uniform buffer descriptor set
		VkDescriptorBufferInfo vxDynamicBufferInfo = {};
		vxDynamicBufferInfo.buffer = vxDynamicBuffer->buffer;
		vxDynamicBufferInfo.offset = 0;
		vxDynamicBufferInfo.range = dynamicAlignment;

		vulkan->descPool->AllocateDescriptor(&vulkan->vxUniformBufferDescriptorLayout,
		                             &vxDynamicUniformDescriptorSet,
		                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		                             nullptr, &vxDynamicBufferInfo, 0, 1);
	}

	void KScene::StaticRenderCallback(VkCommandBuffer buf)
	{
		if (vertexBuffer != nullptr)
		{
			DrawObjects(buf);

			if (!instancedObjects.empty())
			{
				DrawInstancedObjects(buf);
			}
		}
	}

	void KScene::DrawObjects(VkCommandBuffer buf)
	{
		Vulkan::KVulkanPushConstants push = {};
		push.numLights = static_cast<uint32_t>(lights.size());

		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(buf, 0, 1, &vertexBuffer->buffer, offsets);
		vkCmdBindIndexBuffer(buf, indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->mainPipeline->graphicsPipeline);

		uint32_t offset = 0;

		for (uint32_t i = 0; i < objects.size(); ++i)
		{
			UpdateDynamicObjectBuffer(objects[i], i);
			offset = objects[i]->GetMesh()->GetBufferOffset();

			std::array<VkDescriptorSet, 4> descriptorSets = {};
			descriptorSets[0] = uniformDescriptorSet;
			descriptorSets[1] = objects[i]->GetMaterial()->descriptorSet;
			descriptorSets[2] = vxDynamicUniformDescriptorSet;
			descriptorSets[3] = lightsDescriptorSet;

			uint32_t dynamicOffset = i * static_cast<uint32_t>(dynamicAlignment);

			vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                        vulkan->mainPipeline->pipelineLayout,
			                        0, descriptorSets.size(), descriptorSets.data(), 1, &dynamicOffset);

			push.usePhong = (objects[i]->GetMaterial()->properties.material == KM_PHONG) ? VK_TRUE : VK_FALSE;

			vkCmdPushConstants(buf, vulkan->mainPipeline->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   sizeof(Vulkan::KVulkanPushConstants), &push);

			vkCmdDrawIndexed(buf, static_cast<uint32_t>(objects[i]->GetMesh()->indices.size()), 1, 0, offset, 0);
		}
	}

	void KScene::DrawInstancedObjects(VkCommandBuffer buf)
	{
		Vulkan::KVulkanPushConstants push = {};
		push.numLights = static_cast<uint32_t>(lights.size());

		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(buf, 0, 1, &vertexBuffer->buffer, offsets);
		vkCmdBindVertexBuffers(buf, 1, 1, &instanceBuffer->buffer, offsets);
		vkCmdBindIndexBuffer(buf, indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->instancePipeline->graphicsPipeline);

		uint32_t offset = 0;
		uint32_t instances = 0;

		for (uint32_t i = 0; i < instancedObjects.size();)
		{
			IObject *parent = instancedObjects[i]->GetParent();
			offset = parent->GetMesh()->GetBufferOffset();
			instances = parent->GetInstanceCount();

			std::array<VkDescriptorSet, 4> descriptorSets = {};
			descriptorSets[0] = uniformDescriptorSet;
			descriptorSets[1] = parent->GetMaterial()->descriptorSet;
			descriptorSets[2] = vxDynamicUniformDescriptorSet;
			descriptorSets[3] = lightsDescriptorSet;

			uint32_t dynamicOffset = parent->GetIndex() * static_cast<uint32_t>(dynamicAlignment);

			vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                        vulkan->instancePipeline->pipelineLayout,
			                        0, descriptorSets.size(), descriptorSets.data(), 1, &dynamicOffset);

			push.usePhong = (parent->GetMaterial()->properties.material == KM_PHONG) ? VK_TRUE : VK_FALSE;

			vkCmdPushConstants(buf, vulkan->instancePipeline->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   sizeof(Vulkan::KVulkanPushConstants), &push);

			vkCmdDrawIndexed(buf, static_cast<uint32_t>(parent->GetMesh()->indices.size()), instances, 0, offset, i);
			i += instances;

		}
	}

	void KScene::RenderCallback(VkCommandBuffer *buf, uint32_t imageIndex)
	{
		// Commands which cannot be recorded go here.
		vkEndCommandBuffer(*buf);
	}

	void KScene::Update()
	{
		auto swapChainExtent = vulkan->swapChain->swapChainExtent;

		Vulkan::UniformBufferObject ubo = {};
		ubo.view = glm::lookAt(viewPosition, viewPosition + glm::vec3(viewRotation.x, viewRotation.y, viewRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(60.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 1000.0f);
		ubo.proj[1][1] *= -1;
		ubo.worldAmbient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

		Vulkan::LightUniformBufferObject lightUBO = {};
		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			lightUBO.lights[i].pos = glm::vec4(lights[i]->GetPosition(), 1.0f);
			lightUBO.lights[i].color = glm::vec4(lights[i]->color, 1.0f);
			lightUBO.lights[i].specular = glm::vec4(1.0f);
			lightUBO.lights[i].attenuation = glm::vec4(lights[i]->constantAttenuation,
			                                      lights[i]->linearAttenuation,
			                                      lights[i]->quadraticAttenuation, 1.0f);
		}

		memcpy(uniformBuffer->mappedMemory, &ubo, sizeof(ubo));
		memcpy(lightsBuffer->mappedMemory, &lightUBO, sizeof(lightUBO));

		UpdateDynamicUniformBuffers();
	}

	void KScene::UpdateObject(KObject *obj)
	{
		// TODO: Actually update the buffer directly instead of rebuilding the whole thing.
		Actualize();
	}

	void KScene::CreateUniformBuffers()
	{
		VkDeviceSize uniformSize = sizeof(Vulkan::UniformBufferObject);
		VkDeviceSize lightsSize = sizeof(Vulkan::LightUniformBufferObject);
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		size_t minUBOAlignment = vulkan->device->features.VkLimits.minUniformBufferOffsetAlignment;
		if (uniformSize % minUBOAlignment) uniformSize = (uniformSize + minUBOAlignment - 1) & ~(minUBOAlignment - 1);
		if (lightsSize % minUBOAlignment) lightsSize = (lightsSize + minUBOAlignment - 1) & ~(minUBOAlignment - 1);

		uniformBuffer = new Vulkan::KVulkanBuffer(vulkan, uniformSize, usage, flags);
		lightsBuffer = new Vulkan::KVulkanBuffer(vulkan, lightsSize, usage, flags);
	}

	template <typename T>
	Vulkan::KVulkanBuffer *KScene::CreateObjectBuffer(std::vector<T> data, KE_BUFFER_TYPE type)
	{
		Vulkan::KVulkanBuffer *buffer = nullptr;
		VkDeviceSize bufferSize = sizeof(data[0]) * data.size();

		if (type != KT_BUFFER_VERTEX && type != KT_BUFFER_INDEX && type != KT_BUFFER_INSTANCE)
		{
			throw std::runtime_error(WhatWentWrong(KE_UNKNOWN_BUFFER_TYPE));
		}

		if (bufferSize == 0)
		{
			if (type == KT_BUFFER_VERTEX) buffer = dummyVertexBuffer;
			if (type == KT_BUFFER_INDEX) buffer = dummyIndexBuffer;
			if (type == KT_BUFFER_INSTANCE) buffer = dummyInstanceBuffer;
		}
		else
		{
			VkMemoryPropertyFlags memFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkBufferUsageFlags bufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			if (type == KT_BUFFER_VERTEX) bufferFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			if (type == KT_BUFFER_INSTANCE) bufferFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			if (type == KT_BUFFER_INDEX) bufferFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

			auto stagingBuffer = new Vulkan::KVulkanBuffer(vulkan, bufferSize, srcBufferFlags, srcMemFlags);
			stagingBuffer->Fill(data);
			buffer = new Vulkan::KVulkanBuffer(vulkan, bufferSize, bufferFlags, memFlags);
			buffer->Copy(stagingBuffer);
			delete(stagingBuffer);
		}

		return buffer;
	}

	void KScene::CreateDynamicUniformBuffers()
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUBOAlignment = vulkan->device->features.VkLimits.minUniformBufferOffsetAlignment;

		dynamicAlignment = sizeof(Vulkan::vxDynamicUBO);

		if (minUBOAlignment > 0)
		{
			dynamicAlignment = (dynamicAlignment + minUBOAlignment - 1) & ~(minUBOAlignment - 1);
		}

		size_t vxUBOSize = dynamicAlignment * objects.size();

		if (vxUBOSize == 0) vxUBOSize = 1 * dynamicAlignment;

		VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		vxUBO = (Vulkan::vxDynamicUBO*)AlignedAlloc(vxUBOSize, dynamicAlignment);
		vxDynamicBuffer = new Vulkan::KVulkanBuffer(vulkan, vxUBOSize, usage, props);
		vxDynamicBuffer->Map();
		assert(vxUBO);
	}

	void KScene::UpdateDynamicUniformBuffers()
	{
		if (vxUBO != nullptr)
		{
			memcpy(vxDynamicBuffer->mappedMemory, vxUBO, vxDynamicBuffer->size);

			VkMappedMemoryRange vxMemoryRange {};
			vxMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			vxMemoryRange.memory = vxDynamicBuffer->bufferMemory;
			vxMemoryRange.size = vxDynamicBuffer->size;
			vkFlushMappedMemoryRanges(vulkan->device->device, 1, &vxMemoryRange);
		}
	}

	void KScene::UpdateDynamicObjectBuffer(IObject *obj, uint32_t index)
	{
		KMaterialProperties mat = obj->GetMaterial()->properties;

		auto model = (Vulkan::vxDynamicUBO *) (((uint64_t)vxUBO + (index * dynamicAlignment)));
		model->matrix = obj->GetModelMatrix();
		model->material = glm::vec4(mat.specularStrength,
		                            mat.shininess,
		                            mat.ambientStrength,
		                            mat.lightReception);
	}

	void KScene::DeleteEverything()
	{
		for (auto object : objects)
		{
			objLoader->RemoveFromCache(object->GetMesh());
			delete(object);
		}

		objects.clear();

		for (auto material : materials)
		{
			delete(material);
		}

		materials.clear();

		instancedObjects.clear();
	}

	KScene::~KScene()
	{
		DeleteEverything();

		if (!hasUserSetTextureLoader)
		{
			delete(texLoader);
		}

		if (!hasUserSetModelLoader)
		{
			delete(objLoader);
		}

		for (auto light : lights)
		{
			delete(light);
		}

		if (vxUBO) AlignedFree(vxUBO);

		delete(vxDynamicBuffer);
		delete(uniformBuffer);
		delete(lightsBuffer);

		if (indexBuffer != dummyIndexBuffer) delete(indexBuffer);
		if (vertexBuffer != dummyVertexBuffer) delete(vertexBuffer);
		if (instanceBuffer != dummyInstanceBuffer) delete(instanceBuffer);

		delete(dummyIndexBuffer);
		delete(dummyVertexBuffer);
		delete(dummyInstanceBuffer);

		context = nullptr;
	}

	void* KScene::AlignedAlloc(size_t size, size_t alignment)
	{
		void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(size, alignment);
#else
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
#endif
		return data;
	}

	void KScene::AlignedFree(void* data)
	{
#if	defined(_MSC_VER) || defined(__MINGW32__)
		_aligned_free(data);
#else
		free(data);
#endif
	}
}
