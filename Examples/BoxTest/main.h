#ifndef KENGINE_MAIN_H
#define KENGINE_MAIN_H

#include <cmath>
#include <iostream>
#include <sstream>
#include "../../Kitty/include/KEngine.h"
#include <random>

Kitty::KSettings settings;
Kitty::Window::KWindowInfo window = {};
Kitty::KEngine *kitty;
Kitty::KScene *scene;

float oldFPS = 0;
float fps = 0;

void UpdateFPS()
{
	std::stringstream iss;

	fps = kitty->UpdateFPS(1000);

	if (fps != oldFPS)
	{
		oldFPS = fps;
		iss << window.title << " (FPS: " << fps << ")";
		kitty->window->SetWindowTitle(iss.str());
	}
}

bool stopRunning = false;
std::vector<uint32_t> keysDown;

void KeyboardCallback(uint32_t keyCode, uint32_t scanCode,
                      Kitty::Window::Events::KKeyboardAction action, Kitty::Window::Events::KKeyboardModifiers mods)
{
	using namespace Kitty::Window::Events;

	if (action == KA_PRESS && keyCode != KK_KEY_UNKNOWN)
	{
		keysDown.push_back(keyCode);
	}
	else if (action == KA_RELEASE)
	{
		auto index = std::find_if(keysDown.begin(), keysDown.end(),
		                          [&pressedKey = keyCode](uint32_t downKey) -> bool
		                          {
			                          return (pressedKey == downKey);
		                          });

		if (index != keysDown.end()) keysDown.erase(index);

		if (keyCode == KK_KEY_SPACE) window.captureMouse = !window.captureMouse;
		if (keyCode == KK_KEY_CTRL) std::cout << "Position: " << scene->viewPosition.x << ", " << scene->viewPosition.y << "," << scene->viewPosition.z << std::endl;
	}
}

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
glm::vec3 right = {};

void MouseCallback(float x, float y)
{
	if (window.captureMouse)
	{
		horizontalAngle += 0.0002f * float(window.width/2  - x );
		verticalAngle   += 0.0002f * float(window.height/2 - y );

		glm::vec3 direction(
				-std::cos(verticalAngle) * std::sin(horizontalAngle),
				std::cos(verticalAngle) * std::cos(horizontalAngle),
				std::sin(verticalAngle)
		);

		right = glm::vec3(
				-std::sin(horizontalAngle - 3.14f/2.0f),
				std::cos(horizontalAngle - 3.14f/2.0f),
				0
		);

		scene->viewRotation = direction;
	}
}

void DoKeyStuff()
{
	using namespace Kitty::Window::Events;

	for (auto key : keysDown)
	{
		switch (key)
		{
			case KK_KEY_W:
				scene->viewPosition += scene->viewRotation * 0.01f * (1000 / fps);
				break;
			case KK_KEY_S:
				scene->viewPosition -= scene->viewRotation * 0.01f * (1000 / fps);
				break;
			case KK_KEY_D:
				scene->viewPosition += right * 0.01f * (1000 / fps);
				break;
			case KK_KEY_A:
				scene->viewPosition -= right * 0.01f * (1000 / fps);
				break;

			case KK_KEY_ESCAPE:
				stopRunning = true;
				break;

			default:
				break;
		}
	}
}

#endif //KENGINE_MAIN_H
