/**
 * Kitty Engine
 * KHelper.h
 *
 * This is a helper class which contains a number of useful tools
 * for use while working with the Kitty engine.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KHELPER_H
#define KENGINE_KHELPER_H

#include <fstream>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "Vulkan/KVulkan.h"


namespace Kitty
{
	struct KSettings
	{
		const Vulkan::KVulkanDefaults defaults;
		Vulkan::KVulkanSettings vulkan;
		Vulkan::KVulkanGraphicsSettings pipeline;

		KSettings()
		{
			vulkan = defaults.vulkanSettings;
			pipeline = defaults.graphicsPipelineInfo;
		}
	};

	class KHelper
	{
	public:
		/**
		 * \brief Read entire binary file.
		 *
		 * Opens a file as binary and reads the entire contents into a std::vector<char> buffer.
		 *
		 * \return Vector containing all chars of the file, empty vector if file failed to open.
		 */
		std::vector<char> ReadBinaryFile(const std::string& filename);
	};
}


#endif //KENGINE_KHELPER_H
