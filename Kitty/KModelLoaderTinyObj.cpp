/**
 * Kitty Engine
 * KModelLoaderTinyObj.cpp
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

#include <unordered_map>
#include "include/KModelLoaderTinyObj.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../libs/tiny_obj_loader.h"

namespace Kitty
{
	KModelLoaderTinyObj::KModelLoaderTinyObj(KEngine *mainContext, KScene *mainScene, Vulkan::KVulkan *mainVulkan)
	{
		context = mainContext;
		scene = mainScene;
		vulkan = mainVulkan;
	}

	KObject *KModelLoaderTinyObj::LoadModel(std::string filename)
	{
		// If no file was provided, just return the empty object.
		if (filename.empty())
		{
			return new KObject(scene, new KMesh());
		}

		auto mesh = LoadMeshData(filename);
		auto obj = new KObject(scene, mesh);

		return obj;
	}

	KMesh *KModelLoaderTinyObj::LoadMeshData(std::string filename)
	{
		auto mesh = new KMesh();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		auto cache = meshCache.find(filename);

		// Check the mesh cache first
		if (cache != meshCache.end())
		{
			mesh->vertices = cache->second->vertices;
			mesh->indices = cache->second->indices;
			mesh->filename = cache->second->filename;
		}
		else
		{
			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str()))
			{
				throw std::runtime_error(WhatWentWrong(KE_MODEL_LOAD_FAIL));
			}

			for (const auto &shape : shapes)
			{
				for (const auto &index : shape.mesh.indices)
				{
					Vulkan::Vertex vertex = {};

					vertex.pos = {
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.normal = {
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
					};

					if (!attrib.texcoords.empty())
					{
						vertex.texCoord = {
								attrib.texcoords[2 * index.texcoord_index + 0],
								1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
						};
					}

					vertex.color = {1.0f, 1.0f, 1.0f};

					mesh->vertices.push_back(vertex);
					mesh->indices.push_back(static_cast<uint32_t>(mesh->indices.size()));
				}
			}

			meshCache.emplace(filename, mesh);
		}

		return mesh;
	}

	void KModelLoaderTinyObj::ClearCache()
	{
		meshCache.clear();
	}

	void KModelLoaderTinyObj::RemoveFromCache(KMesh *mesh)
	{
		for(auto cache = begin(meshCache); cache != end(meshCache);)
		{
			if (cache->second == mesh)
			{
				cache = meshCache.erase(cache);
				break;
			}
			else
			{
				++cache;
			}
		}
	}

	void KModelLoaderTinyObj::SetVulkanContext(Vulkan::KVulkan *vulkanContext)
	{
		vulkan = vulkanContext;
	}
}
