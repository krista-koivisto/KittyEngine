/**
 * Kitty engine
 * KMesh.cpp
 *
 * Contains mesh data like vertices, indices and normals.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KMesh.h"

#include <utility>

namespace Kitty
{
	KError KMesh::Initialize(std::vector<Vulkan::Vertex> vx, std::vector<uint32_t>ix)
	{
		vertices = std::move(vx);
		indices = std::move(ix);

		return KE_OK;
	}
}
