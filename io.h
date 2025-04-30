#ifndef IO_H
#define IO_H

#include "game.h"

// Configuration
int loadGameConfig(const char *filename, GameWorld *world);

// User input
int processInput(GameWorld *world);
void getPlayerName(char *buffer, int maxLength);

// File I/O
void displayIntroduction(const char *filename);
void saveGameScore(const char *filename, const char *playerName, int score, int reason);
void displayLeaderboard(const char *filename);

#endif // IO_H