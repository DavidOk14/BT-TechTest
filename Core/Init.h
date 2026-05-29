#pragma once

// C++ Includes
#include <iostream>

// SDL3/Vulkan Inlcudes
#include <SDL3/SDL.h>

// User-defined include(s)
#include "../Vulkan/VulkanRenderer.h"
#include "gameLoop.h"
#include "../Vulkan/lve_pipeline.h"

class Init
{
public:
	Init();	// Init constructor to start-up game
	~Init(); // Init destructor to clean-up game

	bool isInitialized() const { return initialized; }  // Game Init Getter
	SDL_Window* getGameWindow() const { return gameWindow;  } // Getter For Game Window
	VulkanRenderer* getGameRenderer() const { return gameRenderer; } // Getter For Game vulkanRenderer
	void printWindowInformation(); // Print Window Information To Console/Log File(s)

private:
	bool initialized = false; // Running State of The Game

	// SDL Window Parameters
	SDL_Window* gameWindow = NULL;	// SDL Window
	const int DEFAULT_WINDOW_WIDTH = 1280; // Default Window Width
	const int DEFAULT_WINDOW_HEIGHT = 720; // Default Window Height

	// Vulkan Based Variables
	VulkanRenderer* gameRenderer = nullptr;
	lve::LvePipeline LvePipeline{"Vulkan/Shaders/simple_shader.vert.spv", "Vulkan/Shaders/simple_shader.frag.spv" };
};