#include "game.h"
#include "player.h"
#include "world.h"
#include "io.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

GameWorld *initializeGame(int width, int height, int difficulty) {
    // Ensure minimum grid size
    if (width < MIN_GRID_SIZE)
        width = MIN_GRID_SIZE;
    if (height < MIN_GRID_SIZE)
        height = MIN_GRID_SIZE;

    // Allocate memory for the game world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    if (world == NULL) {
        printf("Memory allocation failed for game world!\n");
        return NULL;
    }

    // Initialize game world properties
    world->width = width;
    world->height = height;
    world->difficulty = difficulty;
    world->asteroidCollision = 0;
    world->homeDestroyed = 0;

    // Set junk amount based on difficulty
    world->junkTotal = (width * height) / (12 - 3 * difficulty);
    world->junkRemaining = world->junkTotal;

    // Setup grid
    setupGrid(world);

    // Initialize player at random position
    int startX = rand() % width;
    int startY = rand() % height;
    initializePlayer(world, startX, startY);

    // Place space junk
    placeSpaceJunk(world);

    // Initialize asteroid
    initializeAsteroid(world);

    // Place home (win condition)
    placeHome(world);

    return world;
}

void cleanupGame(GameWorld *world) {
    if (world == NULL)
        return;

    // Free the grid
    if (world->grid != NULL) {
        for (int i = 0; i < world->height; i++) {
            if (world->grid[i] != NULL) {
                free(world->grid[i]);
            }
        }
        free(world->grid);
    }

    // Free the world itself
    free(world);
}

void gameLoop(GameWorld *world) {
    int gameRunning = 1;
    int gameOverReason = 0;

    while (gameRunning) {
        // Render current game state
        renderGame(world);

        // Process player input
        int inputResult = processInput(world);
        if (inputResult == -1) {
            // Player wants to quit
            gameRunning = 0;
            continue;
        }

        // Update game state
        updateGameState(world);

        // Check game over conditions
        if (checkGameOver(world, &gameOverReason)) {
            gameRunning = 0;
        }

        // Check win condition
        if (checkWinCondition(world)) {
            gameOverReason = GAME_OVER_WIN;
            gameRunning = 0;
        }

        // Small delay to make the game playable
        msleep(100); // 100 milliseconds
    }

    // Display end game message
    displayGameOverMessage(world, gameOverReason);
}

void updateGameState(GameWorld *world) {
    static int frameCount = 0;
    frameCount++;
    
    // Extra fuel depletion over time (happens on every 5th frame)
    if (frameCount % 5 == 0) {
        depleteFuel(&world->player, 1);
    }
    
    // Asteroid moves on its own occasionally (based on difficulty)
    if (frameCount % (10 - world->difficulty * 2) == 0) {
        moveAsteroid(world);
    }
}

void renderGame(GameWorld *world) {
    clearScreen();
    
    // Display game stats
    printf("SpaceXplorer - Difficulty: %d\n", world->difficulty);
    printf("Fuel: %d | Health: %d | Junk: %d/%d\n\n",
           world->player.fuel, world->player.health,
           world->player.junkCollected, world->junkTotal);
    
    // Draw the grid
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            printf("%c ", world->grid[y][x]);
        }
        printf("\n");
    }
    
    // Display controls
    printf("\nControls: Arrow keys/WASD to move, S for status, U to use junk, Q to quit\n");
}

int checkGameOver(GameWorld *world, int *reason) {
    // Check if player is out of fuel
    if (world->player.fuel <= 0) {
        *reason = GAME_OVER_FUEL;
        return 1;
    }

    // Check for asteroid collision
    if (world->asteroidCollision) {
        *reason = GAME_OVER_ASTEROID;
        return 1;
    }
    
    // Check if home was destroyed
    if (world->homeDestroyed) {
        *reason = GAME_OVER_HOME_DESTROYED;
        return 1;
    }

    return 0; // Game not over
}

int checkWinCondition(GameWorld *world) {
    // Can't win if home is destroyed
    if (world->homeDestroyed) {
        return 0;
    }

    // Win condition: Player reaches home
    if (world->player.position.x == world->homeX &&
        world->player.position.y == world->homeY) {
        return 1;
    }
    return 0;
}

void displayGameOverMessage(GameWorld *world, int reason)
{
    clearScreen();
    printf("\n\n");

    switch (reason)
    {
    case GAME_OVER_FUEL:
        printf("Game Over: You ran out of fuel! Your ship drifts endlessly through space...\n");
        break;
    case GAME_OVER_ASTEROID:
        printf("Game Over: Your ship was hit by an asteroid!\n");
        break;
    case GAME_OVER_HOME_DESTROYED:
        printf("Game Over: Your home base was destroyed by an asteroid! There's nowhere to return to...\n");
        break;
    case GAME_OVER_WIN:
        printf("Congratulations! You've made it home safely!\n");
        break;
    }

    printf("You collected %d pieces of space junk.\n", world->player.junkCollected);

    // Save score for ALL game outcomes, not just wins
    char playerName[50];
    getPlayerName(playerName, 50);
    saveGameScore("scores.txt", playerName, world->player.junkCollected, reason);

    // Display leaderboard for all game outcomes
    printf("\nLeaderboard:\n");
    displayLeaderboard("scores.txt");

    printf("\nPress any key to continue...");
    getch();
}

void collectJunk(GameWorld *world) {
    world->player.junkCollected++;
    world->junkRemaining--;
    printf("\nYou collected a piece of space junk!\n");
}

void useJunk(GameWorld *world, int amount) {
    if (world->player.junkCollected >= amount) {
        world->player.junkCollected -= amount;
        
        // Refuel and repair based on junk used
        world->player.fuel += 20 * amount;
        world->player.health += 10 * amount;
        
        if (world->player.health > 100)
            world->player.health = 100;
            
        printf("\nYou recycled space junk to refuel and repair your ship!\n");
    } else {
        printf("\nNot enough space junk to recycle!\n");
    }
}

int displayMenu(void) {
    clearScreen();
    
    printf("\n");
    printf("   ____                     _____       _                     \n");
    printf("  / ___| _ __   __ _  ___ | ____|_  __| |_ __  _ __ ___ _ __ \n");
    printf("  \\___ \\| '_ \\ / _` |/ _ \\|  _| \\ \\/ /| | '_ \\| '__/ _ \\ '__|\n");
    printf("   ___) | |_) | (_| | (_) | |___ >  < | | |_) | | |  __/ |   \n");
    printf("  |____/| .__/ \\__,_|\\___/|_____/_/\\_\\|_| .__/|_|  \\___|_|   \n");
    printf("        |_|                             |_|                   \n\n");
    
    printf("1. Start New Game\n");
    printf("2. View Leaderboard\n");
    printf("3. Exit Game\n\n");
    
    printf("Enter your choice (1-3): ");
    
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n'); // Clear input buffer
    
    return choice;
}

int getDifficulty(void) {
    clearScreen();
    printf("Select Difficulty:\n\n");
    printf("1. Easy   - More fuel, slower asteroid\n");
    printf("2. Medium - Average fuel, normal asteroid\n");
    printf("3. Hard   - Less fuel, faster asteroid\n\n");
    
    printf("Enter your choice (1-3): ");
    
    int difficulty;
    scanf("%d", &difficulty);
    while (getchar() != '\n'); // Clear input buffer
    
    // Validate input
    if (difficulty < 1 || difficulty > 3)
        difficulty = 2; // Default to medium
        
    return difficulty;
}