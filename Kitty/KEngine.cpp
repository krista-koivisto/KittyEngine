/**
 * Kitty Engine
 * KEngine.cpp
 *
 * This is the core interface for the Kitty engine. This class is
 * the starting point when using the engine. It is also responsible
 * for providing an interface to the Kitty engine Vulkan abstraction
 * layer as well as setting up the chosen window manager.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KEngine.h"
#include "include/KWindowGLFW.h"

namespace Kitty
{
	KEngine::KEngine(Window::KWindowInfo *windowInfo,
	                 KSettings *kittySettings,
	                 Window::IWindow *windowManager)
	{
		if (kittySettings != nullptr)
		{
			settings = defaults.ObtainValues(&kittySettings->vulkan, &settings);
			graphicsSettings = defaults.ObtainValues(&kittySettings->pipeline, &graphicsSettings);
		}

		if (windowManager == nullptr)
		{
			windowManager = new Window::KWindowGLFW();
			hasUserSetWindowManager = false;
		}

		window = windowManager;

		if (windowInfo != nullptr && windowInfo->canScale)
		{
			// Assign the window resize callback function (doing the lambda, the forbidden function!)
			windowInfo->resizeCallback = [this](Window::IWindow *window, int width, int height)
			{
				OnWindowResize(window, width, height);
			};
		}

		if (window->Create(windowInfo) != Window::KW_OK)
		{
			throw std::runtime_error(WhatWentWrong(KE_WINDOW_CREATE_FAIL));
		}

		KError err = InitializeVulkan();
		if (err != KE_OK) throw std::runtime_error(WhatWentWrong(err));
	}

	KError KEngine::InitializeVulkan()
	{
		settings.appInfo.pEngineName = "Kitty Engine";
		settings.appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);

		if (settings.outdatedSwapChainCallback == nullptr)
		{
			settings.outdatedSwapChainCallback = [this](){ vulkan->RecreateSwapChain(); };
		}

		vulkan = new Vulkan::KVulkan(window, &settings, &graphicsSettings);

		return KE_OK;
	}

	KError KEngine::IsRunning()
	{
		if (window->Update() == Window::KW_OK)
		{
			vulkan->DrawFrame();

			return KE_OK;
		}
		else
		{
			vulkan->FinishDrawing();
			return KE_STOP;
		}
	}

	float KEngine::UpdateFPS(uint32_t fpsUpdateFrequency)
	{
		delta = std::chrono::high_resolution_clock::now();
		++fpsCounter;

		if (std::chrono::duration_cast<std::chrono::milliseconds>(delta - lastFPSUpdate).count() >= fpsUpdateFrequency)
		{
			lastFPSUpdate = std::chrono::high_resolution_clock::now();
			fps = (float)fpsCounter * (1000.0f / fpsUpdateFrequency);
			fpsCounter = 0;
		}

		return fps;
	}

	void KEngine::OnWindowResize(Window::IWindow *window, int width, int height)
	{
		vulkan->FinishDrawing();

		if (vulkan->outdatedSwapChainCallback != nullptr)
		{
			vulkan->outdatedSwapChainCallback();
		}
		else
		{
			vulkan->RecreateSwapChain();
		}
	}

	KScene *KEngine::CreateScene(ITextureLoader *textureLoader)
	{
		auto *scene = new KScene(this, vulkan, textureLoader);
		scenes.push_back(scene);
		return scene;
	}

	KEngine::~KEngine()
	{
		for (auto scene : scenes)
		{
			delete(scene);
		}

		delete(vulkan);
		vulkan = nullptr;

		if (!hasUserSetWindowManager)
		{
			delete(window);
			window = nullptr;
		}
	}
}
