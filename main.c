#include "game.h"
#include "io.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    int running = 1;
    
    while (running) {
        // Display menu
        int choice = displayMenu();
        
        switch (choice) {
            case 1: // Start new game
                {
                    clearScreen();
                    displayIntroduction("intro.txt");
                    
                    // Get difficulty
                    int difficulty = getDifficulty();
                    
                    // Initialize game
                    GameWorld *world = NULL;
                    
                    // Try to load from config file first
                    world = (GameWorld *)malloc(sizeof(GameWorld));
                    if (world != NULL) {
                        if (loadGameConfig("config.txt", world)) {
                            // Configuration loaded successfully
                            world->difficulty = difficulty;
                        } else {
                            // Use default settings
                            free(world);
                            world = initializeGame(20, 20, difficulty);
                        }
                        
                        // Start game loop
                        if (world != NULL) {
                            gameLoop(world);
                            cleanupGame(world);
                        }
                    }
                }
                break;
                
            case 2: // View leaderboard
                clearScreen();
                printf("=== LEADERBOARD ===\n\n");
                displayLeaderboard("scores.txt");
                break;
                
            case 3: // Exit
                running = 0;
                break;
                
            default:
                printf("Invalid choice! Press any key to continue...");
                getch();
                break;
        }
    }
    
    return 0;
}