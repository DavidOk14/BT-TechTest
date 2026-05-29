#pragma once

// User-defined include(s)
#include "Init.h"

class gameLoop
{
public:
	gameLoop(VulkanRenderer* renderer); // gameLoop constructor
	~gameLoop(); // gameLoop destructor

	bool getRunningState() const { return running; } // Getter For Running State

	void run(); // Function Containing Main Loop

private:
	bool running = false;	// Game Loop Boolean
	SDL_Event event;	// Catches SDL Events
	VulkanRenderer* gameRenderer;  // Vulkan renderer instance

};

