/**
 * Kitty engine
 * KMesh.h
 *
 * Contains mesh data like vertices, indices and normals.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANMODEL_H
#define KENGINE_KVULKANMODEL_H

#include <vulkan/vulkan.h>
#include <cstring>
#include "Vulkan/KVulkan.h"
#include "KError.h"

using namespace Kitty::Error;

namespace Kitty
{
		class KMesh
		{
		private:
			uint32_t bufferOffset = 0;

		public:
			KMesh() = default;
			~KMesh() = default;

			std::vector<Vulkan::Vertex> vertices = {};
			std::vector<uint32_t> indices = {};
			std::string filename = "";

			/**
			 * \brief Copy vertex and index data to the mesh.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(std::vector<Vulkan::Vertex> vx, std::vector<uint32_t> ix);

			/**
			 * \brief Set mesh offset in vertex buffer.
			 *
			 * \param offset Vertex buffer offset.
			 */
			void SetBufferOffset(uint32_t offset) { bufferOffset = offset; }

			/**
			 * \brief Get mesh offset in vertex buffer.
			 *
			 * \return Mesh vertex buffer offset.
			 */
			uint32_t GetBufferOffset() { return bufferOffset; }
		};
}


#endif //KENGINE_KVULKANMODEL_H
