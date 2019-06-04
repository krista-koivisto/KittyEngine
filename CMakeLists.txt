cmake_minimum_required(VERSION 3.8)
project(kittyengine)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${glfw3_INCLUDE_DIRS})

set(SOURCE_FILES Kitty/KEngine.cpp Kitty/include/KEngine.h Kitty/KError.cpp Kitty/include/KError.h Kitty/include/IWindow.h Kitty/KWindowGLFW.cpp Kitty/include/KWindowGLFW.h Kitty/KScene.cpp Kitty/include/KScene.h Kitty/include/KVectors.h Kitty/KHelper.cpp Kitty/include/KHelper.h Kitty/Vulkan/KVulkan.cpp Kitty/include/Vulkan/KVulkan.h Kitty/Vulkan/KVulkanDevice.cpp Kitty/include/Vulkan/KVulkanDevice.h Kitty/include/Vulkan/KVulkanDefaults.h Kitty/Vulkan/KVulkanSwapChain.cpp Kitty/include/Vulkan/KVulkanSwapChain.h Kitty/Vulkan/KVulkanImageView.cpp Kitty/include/Vulkan/KVulkanImageView.h Kitty/Vulkan/KVulkanGraphicsPipeline.cpp Kitty/include/Vulkan/KVulkanGraphicsPipeline.h Kitty/include/Vulkan/KVulkanHelpers.h Kitty/Vulkan/KVulkanFramebuffer.cpp Kitty/include/Vulkan/KVulkanFramebuffer.h Kitty/Vulkan/KVulkanCommandPool.cpp Kitty/include/Vulkan/KVulkanCommandPool.h Kitty/Vulkan/KVulkanTexture.cpp Kitty/include/Vulkan/KVulkanTexture.h Kitty/KMesh.cpp Kitty/include/KMesh.h Kitty/Vulkan/KVulkanBuffer.cpp Kitty/include/Vulkan/KVulkanBuffer.h Kitty/Vulkan/KVulkanDescriptorPool.cpp Kitty/include/Vulkan/KVulkanDescriptorPool.h libs/stb_image.h Kitty/KTextureLoaderSTB.cpp Kitty/include/KTextureLoaderSTB.h Kitty/include/ITextureLoader.h Kitty/KObject.cpp Kitty/include/KObject.h Kitty/Vulkan/KVulkanImage.cpp Kitty/include/Vulkan/KVulkanImage.h Kitty/KModelLoaderTinyObj.cpp Kitty/include/KModelLoaderTinyObj.h libs/tiny_obj_loader.h Kitty/KMaterial.cpp Kitty/include/KMaterial.h Kitty/KLight.cpp Kitty/include/KLight.h Kitty/Vulkan/KVulkanRenderPass.cpp Kitty/include/Vulkan/KVulkanRenderPass.h Kitty/KInstancedObject.cpp Kitty/include/KInstancedObject.h Kitty/IObject.cpp Kitty/include/IObject.h)

add_library(kittyengine ${SOURCE_FILES})

target_link_libraries (kittyengine glfw)
target_link_libraries (kittyengine Vulkan::Vulkan)

# Box Test

project(BoxTest)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${glfw3_INCLUDE_DIRS})

set(SOURCE_FILES Examples/BoxTest/main.cpp Examples/BoxTest/main.h)

add_executable(BoxTest ${SOURCE_FILES})

target_link_libraries(BoxTest kittyengine)
target_link_libraries (BoxTest glfw)
target_link_libraries (BoxTest Vulkan::Vulkan)
