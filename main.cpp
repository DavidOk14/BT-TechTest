#include "Core/gameLoop.h"

int main(int argc, char* argv[])
{
    // Boot up the game
    Init initGame; 

    // Verify game is running before executing loop
    if (!initGame.isInitialized())
    {
        std::cerr << "Failed To Initialize Game." << std::endl;
        return -1;
    }

    // Create game loop instance
    gameLoop mainGame(initGame.getGameRenderer());

    // Run game
    mainGame.run();

    return 0;
}
