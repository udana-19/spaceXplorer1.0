#ifndef WORLD_H
#define WORLD_H

#include "game.h"

// World grid setup
void setupGrid(GameWorld *world);

// Object placement
void placeSpaceJunk(GameWorld *world);
void initializeAsteroid(GameWorld *world);
void placeHome(GameWorld *world);

// Asteroid movement
void moveAsteroid(GameWorld *world);

#endif // WORLD_H