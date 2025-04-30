#include "player.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"

void initializePlayer(GameWorld *world, int startX, int startY) {
    world->player.position.x = startX;
    world->player.position.y = startY;
    
    // Set initial player stats based on difficulty
    world->player.health = 100;
    
    switch (world->difficulty) {
        case 1: // Easy
            world->player.fuel = 200;
            break;
        case 2: // Medium
            world->player.fuel = 150;
            break;
        case 3: // Hard
            world->player.fuel = 100;
            break;
        default:
            world->player.fuel = 150;
    }
    
    world->player.junkCollected = 0;
    
    // Place player on grid
    world->grid[startY][startX] = 'P';
}

int movePlayer(GameWorld *world, int deltaX, int deltaY) {
    // Calculate new position
    int newX = world->player.position.x + deltaX;
    int newY = world->player.position.y + deltaY;
    
    // Check boundaries
    if (newX < 0 || newX >= world->width || newY < 0 || newY >= world->height)
        return 0;
    
    // Get what's at the target position
    char targetCell = world->grid[newY][newX];
    
    // Handle different cell types
    switch (targetCell) {
        case '*':  // Space junk
            collectJunk(world);
            break;
        case 'A':  // Asteroid
            // Collision with asteroid - game over
            world->grid[newY][newX] = 'X';
            world->asteroidCollision = 1;
            return 1;
        case 'H':  // Home
            // Win condition handled in game loop
            break;
    }
    
    // Update the grid
    world->grid[world->player.position.y][world->player.position.x] = ' ';
    world->grid[newY][newX] = 'P';
    
    // Update player position
    world->player.position.x = newX;
    world->player.position.y = newY;
    
    // Use fuel
    depleteFuel(&world->player, world->difficulty);
    
    // Move asteroid after player moves
    moveAsteroid(world);
    
    return 1;
}

void depleteFuel(Player *player, int amount) {
    player->fuel -= amount;
    if (player->fuel < 0) {
        player->fuel = 0;
    }
}

void displayShipStatus(Player *player) {
    clearScreen();
    printf("===== SHIP STATUS =====\n\n");
    
    printf("Hull Integrity: %d%%\n", player->health);
    printf("Fuel Reserves: %d units\n", player->fuel);
    printf("Space Junk Collected: %d pieces\n", player->junkCollected);
    
    printf("\nPress any key to continue...");
    getch();
}