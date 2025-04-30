#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "world.h"
#include "player.h"
#include "io.h"

// Test counter variables
int tests_run = 0;
int tests_passed = 0;

// Simple test reporting macro
#define TEST(test_name)                         \
    printf("Running test: %s...\n", test_name); \
    tests_run++;                                \
    if (

#define END_TEST )          \
    {                       \
        printf("PASSED\n"); \
        tests_passed++;     \
    }                       \
    else                    \
    {                       \
        printf("FAILED\n"); \
    }

void test_setupGrid(void)
{
    // Create a test world
    GameWorld world;
    world.width = 10;
    world.height = 8;

    // Call function to test
    setupGrid(&world);

    // Verify grid was created correctly
    TEST("Grid Creation")
    world.grid != NULL END_TEST

                      // Check grid dimensions
                      TEST("Grid Dimensions") world.grid[0] != NULL &&world.grid[7] != NULL END_TEST

                                                                                           // Check grid initialization (all spaces)
                                                                                           TEST("Grid Initialization") world.grid[3][3] == ' ' &&
        world.grid[5][5] == ' ' END_TEST

            // Clean up
            for (int i = 0; i < world.height; i++)
    {
        free(world.grid[i]);
    }
    free(world.grid);
}

void test_playerMovement(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;
    world->difficulty = 1;
    world->asteroidCollision = 0;
    world->homeDestroyed = 0;

    // Setup grid
    setupGrid(world);

    // Initialize player
    initializePlayer(world, 5, 5);

    // Verify initial position
    TEST("Initial Player Position")
    world->player.position.x == 5 &&
        world->player.position.y == 5 &&
        world->grid[5][5] == 'P' END_TEST

            // Test moving player up
            int moveResult = movePlayer(world, 0, -1);

    TEST("Player Movement Up")
    moveResult == 1 &&
        world->player.position.x == 5 &&
        world->player.position.y == 4 &&
        world->grid[4][5] == 'P' &&
        world->grid[5][5] == ' ' END_TEST

                                 // Clean up
                                 cleanupGame(world);
}

void test_junkCollection(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;
    world->difficulty = 1;
    world->junkTotal = 5;
    world->junkRemaining = 5;
    world->asteroidCollision = 0;
    world->homeDestroyed = 0;

    // Setup grid
    setupGrid(world);

    // Initialize player
    initializePlayer(world, 5, 5);

    // Place junk next to player
    world->grid[5][6] = '*';

    // Verify initial junk count
    TEST("Initial Junk Count")
    world->player.junkCollected == 0 &&
        world->junkRemaining == 5 END_TEST

                                    // Test moving player to collect junk
                                    movePlayer(world, 1, 0); // Move right

    TEST("Junk Collection")
    world->player.junkCollected == 1 &&
        world->junkRemaining == 4 END_TEST

                                    // Clean up
                                    cleanupGame(world);
}

void test_fuelDepletion(void)
{
    // Create a test player
    Player player;
    player.fuel = 100;

    // Test fuel depletion
    depleteFuel(&player, 20);

    TEST("Normal Fuel Depletion")
    player.fuel == 80 END_TEST

                       // Test fuel depletion beyond zero
                       depleteFuel(&player, 100);

    TEST("Fuel Depletion Below Zero")
    player.fuel == 0 // Should stop at zero, not go negative
        END_TEST
}

void test_asteroidMovement(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;

    // Setup grid
    setupGrid(world);

    // Initialize asteroid
    world->asteroid.position.x = 3;
    world->asteroid.position.y = 3;
    world->asteroid.deltaX = 1; // Moving right
    world->asteroid.deltaY = 0; // Not moving vertically
    world->grid[3][3] = 'A';

    // Test asteroid movement
    moveAsteroid(world);

    TEST("Basic Asteroid Movement")
    world->asteroid.position.x == 4 &&
        world->asteroid.position.y == 3 &&
        world->grid[3][3] == ' ' &&
        world->grid[3][4] == 'A' END_TEST

                                 // Clean up
                                 cleanupGame(world);
}

void test_wrapAround(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;

    // Setup grid
    setupGrid(world);

    // Initialize asteroid at edge
    world->asteroid.position.x = 9; // Right edge
    world->asteroid.position.y = 5;
    world->asteroid.deltaX = 1; // Moving right (should wrap)
    world->asteroid.deltaY = 0;
    world->grid[5][9] = 'A';

    // Test asteroid wrap-around
    moveAsteroid(world);

    TEST("Asteroid Horizontal Wrap-Around")
    world->asteroid.position.x == 0 &&
        world->asteroid.position.y == 5 &&
        world->grid[5][9] == ' ' &&
        world->grid[5][0] == 'A' END_TEST

                                 // Clean up
                                 cleanupGame(world);
}

void test_gameOverConditions(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;
    world->player.fuel = 0; // No fuel
    world->asteroidCollision = 0;
    world->homeDestroyed = 0;

    // Test out of fuel condition
    int reason = 0;
    int isGameOver = checkGameOver(world, &reason);

    TEST("Game Over - No Fuel")
    isGameOver == 1 &&reason == GAME_OVER_FUEL END_TEST

                                    // Test asteroid collision
                                    world->player.fuel = 100; // Reset fuel
    world->asteroidCollision = 1;                             // Set collision flag

    reason = 0;
    isGameOver = checkGameOver(world, &reason);

    TEST("Game Over - Asteroid Collision")
    isGameOver == 1 && reason == GAME_OVER_ASTEROID
                                     END_TEST

                                         // Clean up
                                         free(world);
}

void test_winCondition(void)
{
    // Create a test world
    GameWorld *world = (GameWorld *)malloc(sizeof(GameWorld));
    world->width = 10;
    world->height = 10;
    world->homeDestroyed = 0;

    // Setup grid
    setupGrid(world);

    // Initialize player
    world->player.position.x = 5;
    world->player.position.y = 5;

    // Set home at different location
    world->homeX = 7;
    world->homeY = 7;

    // Test - player not at home
    int hasWon = checkWinCondition(world);

    TEST("Win Condition - Not Met")
    hasWon == 0 END_TEST

                  // Move player to home
                  world->player.position.x = world->homeX;
    world->player.position.y = world->homeY;

    // Test - player at home
    hasWon = checkWinCondition(world);

    TEST("Win Condition - Met")
    hasWon == 1 END_TEST

                  // Clean up
                  cleanupGame(world);
}

// Main function to run all tests
int main(void)
{
    printf("Starting SpaceXplorer unit tests...\n\n");

    // Run all tests
    test_setupGrid();
    test_playerMovement();
    test_junkCollection();
    test_fuelDepletion();
    test_asteroidMovement();
    test_wrapAround();
    test_gameOverConditions();
    test_winCondition();

    // Report results
    printf("\nTests complete: %d passed out of %d run\n", tests_passed, tests_run);
    return tests_run - tests_passed;
}