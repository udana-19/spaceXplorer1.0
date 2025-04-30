#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Game constants
#define MIN_GRID_SIZE 18
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

// Game over reasons
#define GAME_OVER_FUEL 1
#define GAME_OVER_ASTEROID 2
#define GAME_OVER_HOME_DESTROYED 3
#define GAME_OVER_WIN 4

// Position structure
typedef struct {
    int x;
    int y;
} Position;

// Player data
typedef struct {
    Position position;
    int health;
    int fuel;
    int junkCollected;
} Player;

// Asteroid data
typedef struct {
    Position position;
    int deltaX;
    int deltaY;
} Asteroid;

// Game world
typedef struct {
    int width;
    int height;
    char **grid;
    Player player;
    Asteroid asteroid;
    int difficulty;
    int junkTotal;
    int junkRemaining;
    int homeX, homeY;
    int asteroidCollision;
    int homeDestroyed;
} GameWorld;

// Game initialization and cleanup
GameWorld* initializeGame(int width, int height, int difficulty);
void cleanupGame(GameWorld *world);

// Game loop and state management
void gameLoop(GameWorld *world);
void updateGameState(GameWorld *world);
int checkGameOver(GameWorld *world, int *reason);
int checkWinCondition(GameWorld *world);
void displayGameOverMessage(GameWorld *world, int reason);

// Rendering
void renderGame(GameWorld *world);

// Resource management
void collectJunk(GameWorld *world);
void useJunk(GameWorld *world, int amount);
void displayShipStatus(Player *player);

// Menu functions
int displayMenu(void);
int getDifficulty(void);

#endif // GAME_H