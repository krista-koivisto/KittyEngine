/**
 * Kitty Engine
 * KHelper.cpp
 *
 * This is a helper class which contains a number of useful tools
 * for use while working with the Kitty engine.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KHelper.h"

namespace Kitty
{
	std::vector<char> KHelper::ReadBinaryFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			return std::vector<char>();

		auto fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}