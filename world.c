#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include "platform.h"

void setupGrid(GameWorld *world) {
    // Allocate memory for rows
    world->grid = (char **)malloc(world->height * sizeof(char *));
    if (world->grid == NULL) {
        printf("Memory allocation failed for grid rows!\n");
        return;
    }
    
    // Allocate memory for columns and initialize
    for (int i = 0; i < world->height; i++) {
        world->grid[i] = (char *)malloc(world->width * sizeof(char));
        if (world->grid[i] == NULL) {
            printf("Memory allocation failed for grid column %d!\n", i);
            return;
        }
        
        // Initialize all cells as empty
        for (int j = 0; j < world->width; j++) {
            world->grid[i][j] = ' ';
        }
    }
}

void placeSpaceJunk(GameWorld *world) {
    int junkPlaced = 0;
    
    while (junkPlaced < world->junkTotal) {
        int x = rand() % world->width;
        int y = rand() % world->height;
        
        // Check if the cell is empty
        if (world->grid[y][x] == ' ') {
            world->grid[y][x] = '*';
            junkPlaced++;
        }
    }
}

void initializeAsteroid(GameWorld *world) {
    int placed = 0;
    
    // Try to place asteroid away from player
    while (!placed) {
        int x = rand() % world->width;
        int y = rand() % world->height;
        
        // Keep asteroid at least 3 cells away from player at start
        int dx = abs(x - world->player.position.x);
        int dy = abs(y - world->player.position.y);
        
        if (dx > 3 && dy > 3 && world->grid[y][x] == ' ') {
            world->grid[y][x] = 'A';
            world->asteroid.position.x = x;
            world->asteroid.position.y = y;
            placed = 1;
        }
    }
    
    // Set random trajectory for asteroid
    do {
        world->asteroid.deltaX = (rand() % 3) - 1; // -1, 0, or 1
        world->asteroid.deltaY = (rand() % 3) - 1; // -1, 0, or 1
    } while (world->asteroid.deltaX == 0 && world->asteroid.deltaY == 0);
}

void placeHome(GameWorld *world) {
    int placed = 0;
    
    // Try to place home far from player
    while (!placed) {
        int x = rand() % world->width;
        int y = rand() % world->height;
        
        // Keep home on opposite side of grid from player
        int dx = abs(x - world->player.position.x);
        int dy = abs(y - world->player.position.y);
        
        if (dx > world->width/2 && dy > world->height/2 && world->grid[y][x] == ' ') {
            world->grid[y][x] = 'H';
            world->homeX = x;
            world->homeY = y;
            placed = 1;
        }
    }
}

void moveAsteroid(GameWorld *world) {
    // Calculate new position
    int newX = world->asteroid.position.x + world->asteroid.deltaX;
    int newY = world->asteroid.position.y + world->asteroid.deltaY;
    
    // Wrap around screen edges
    if (newX < 0) newX = world->width - 1;
    if (newX >= world->width) newX = 0;
    if (newY < 0) newY = world->height - 1;
    if (newY >= world->height) newY = 0;
    
    // Remove from old position
    world->grid[world->asteroid.position.y][world->asteroid.position.x] = ' ';
    
    // Check for collisions
    if (world->grid[newY][newX] == 'P') {
        // Hit player - game over
        world->grid[newY][newX] = 'X';
        world->asteroidCollision = 1;
        return;
    } else if (world->grid[newY][newX] == 'H') {
        // Hit home - home destroyed
        world->grid[newY][newX] = 'A';
        world->homeDestroyed = 1;
        return;
    }
    
    // Update position
    world->grid[newY][newX] = 'A';
    world->asteroid.position.x = newX;
    world->asteroid.position.y = newY;
}