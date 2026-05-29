#include "Init.h"

Init::Init()
{
	if (!initialized)
	{
		// Initialize SDL_Video
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS))
		{
			std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return;
		}

		// Create Game Window With Vulkan Context + Resizability
			gameWindow = SDL_CreateWindow("BT-TechTest", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
			// Exit if the game has an error creating window
			if (!gameWindow)
			{
				// TODO: Log error
				std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
				SDL_Quit();
				return;
			}

		// Print SDL Window Information
			printWindowInformation();

		// Initialize Vulkan
			if (!SDL_Vulkan_LoadLibrary(nullptr)) // Load The SDL3 Vulkan Libraries
			{
				// TODO: Log error
				std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
				SDL_Quit();
			}
			else
				std::cout << "Successfully Loaded SDL3 Vulkan Library Loader\n" << std::endl;

		gameRenderer = new VulkanRenderer(gameWindow);

		// Set initialized flag to true -- initialization phase finished.
		initialized = true;
	}
}

void Init::printWindowInformation()
{
	// Create & Set Variables for Window Size
	int windowWidth, windowHeight;
	SDL_GetWindowSize(gameWindow, &windowWidth, &windowHeight);

	// Create & Set Variables for Window Size
	int windowX, windowY;
	SDL_GetWindowPosition(gameWindow, &windowX, &windowY);

	// Print Window Details
	std::cout << "[SDL3] Window Dimensions: " << windowWidth << " x " << windowHeight << std::endl;
	std::cout << "[SDL3] Window On Display #" << SDL_GetDisplayForWindow(gameWindow) << std::endl;
	std::cout << "[SDL3] Window Position: " << windowX << ", " << windowY << std::endl;
	std::cout << "[SDL3] Window ID: " << SDL_GetWindowID(gameWindow) << std::endl;
	std::cout << "[SDL3] Window Title: " << SDL_GetWindowTitle(gameWindow) << std::endl;
	std::cout << std::endl;
}

Init::~Init()
{
	if (initialized)
	{
		// Cleanup
		SDL_Vulkan_UnloadLibrary();
		std::cout << "Destroying SDL3 Vulkan Library Loader..." << std::endl;

		delete gameRenderer;
		std::cout << "Destroying Game Renderer..." << std::endl;

		SDL_DestroyWindow(gameWindow);
		SDL_Quit();

		std::cout << "Destroying SDL Window..." << std::endl;

		// Set initialized flag to false before exiting
		initialized = false;
	}
}
