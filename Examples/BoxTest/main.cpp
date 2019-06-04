#include "main.h"

int main()
{
	try
	{
		// Window settings
		window.title = "Hello Kitty";
		window.width = 1024;
		window.height = 768;
		window.canScale = true;
		window.keyboardCallback = KeyboardCallback;
		window.mouseMoveCallback = MouseCallback;

		settings.vulkan.swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // V-Sync

		// Initialize Kitty
		kitty = new Kitty::KEngine(&window, &settings);

		// Create a scene
		scene = kitty->CreateScene();
		scene->viewPosition = glm::vec3(2, 5, 2);
		scene->viewRotation = glm::vec3(-2, -5, -2);

		// Sky dome
		auto sky = scene->LoadModel("../../Models/sphere.obj");
		sky->SetMaterial(scene->LoadImageTexture("../../Textures/nightsky.jpg"));
		sky->GetMaterial()->properties.ambientStrength = 0.2f;
		sky->SetScale(200.0f);

		auto txt = scene->LoadImageTexture("../../Textures/Kitty512.png");
		txt->properties.material = Kitty::KM_PHONG;

		// A box...
		auto yay = scene->LoadModel("../../Models/box.obj");
		yay->SetPosition(glm::vec3(-50, -50, 0));
		yay->SetMaterial(txt);
		yay->SetScale(0.1f);

		txt->properties.ambientStrength = 0.05f;

		srand(static_cast<unsigned int>(time(0)));

		// And 99,999 of its friends...
		for (int i = 0; i < 99999; ++i)
		{
			auto inst = yay->CreateInstance();
			inst->SetPosition(glm::vec3(rand() % 200000 / 250.0f, rand() % 200000 / 250.f, rand() % 200000 / 250.0f));
		}

		// Let there be light!
		auto licht = scene->CreateLight();
		licht->color = glm::vec3(1.0f, 1.0f, 0.8f);
		licht->quadraticAttenuation = 0.2f;

		// And another one, because why not?
		auto licht2 = scene->CreateLight();
		licht2->color = glm::vec3(0.2f, 0.0f, 0.05f);
		licht2->SetPosition(glm::vec3(30, 0, 50));
		licht2->linearAttenuation = 0.02f;

		// Yayyy!!
		auto licht3 = scene->CreateLight();
		licht3->color = glm::vec3(0.4f, 0.4f, 0.1f);
		licht3->SetPosition(glm::vec3(-20, 0, 50));
		licht3->linearAttenuation = 0.04f;

		// The scene must now be actualized. This is how loaded resources are handed over to Vulkan.
		scene->Actualize();

		// Now we go watch the pretties!
		while (kitty->IsRunning() && !stopRunning)
		{
			scene->Update();
			UpdateFPS();
			DoKeyStuff();
		}

		delete (kitty);
	}
	catch(const std::runtime_error& err)
	{
		std::cerr << "Error: " << err.what() << std::endl;
	}

	return 0;
}
