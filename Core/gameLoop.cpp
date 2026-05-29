#include "gameLoop.h"

gameLoop::gameLoop(VulkanRenderer* renderer) : running(true), gameRenderer(renderer)
{
    if (renderer == nullptr)
    {
        std::cerr << "Renderer not initialized!" << std::endl;
        SDL_Quit();
    }
}

gameLoop::~gameLoop()
{
    running = false;
    std::cout << "Ending game loop" << std::endl;
}

void gameLoop::run()
{
    // Main loop
    running = true;

    while (running)
    {
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }
    }

    //gameRenderer->Render();
}
