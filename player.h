#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

// Player initialization and movement
void initializePlayer(GameWorld *world, int startX, int startY);
int movePlayer(GameWorld *world, int deltaX, int deltaY);
void depleteFuel(Player *player, int amount);

#endif // PLAYER_H