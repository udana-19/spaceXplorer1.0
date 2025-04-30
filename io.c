#include "io.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loadGameConfig(const char *filename, GameWorld *world)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open configuration file: %s\n", filename);
        return 0;
    }

    // Read basic config - width height difficulty
    if (fscanf(file, "%d %d %d", &world->width, &world->height, &world->difficulty) != 3)
    {
        fclose(file);
        return 0;
    }

    fclose(file);

    // Initialize flags
    world->asteroidCollision = 0;
    world->homeDestroyed = 0;

    // Initialize the rest of the world
    setupGrid(world);

    // Set junk amount based on difficulty
    world->junkTotal = (world->width * world->height) / (12 - 3 * world->difficulty);
    world->junkRemaining = world->junkTotal;

    // Initialize player at random position
    int startX = rand() % world->width;
    int startY = rand() % world->height;
    initializePlayer(world, startX, startY);

    // Place space junk
    placeSpaceJunk(world);

    // Initialize asteroid
    initializeAsteroid(world);

    // Place home (win condition)
    placeHome(world);

    return 1;
}

int processInput(GameWorld *world)
{
    if (kbhit())
    {
        int key = getch();

// Handle arrow keys (platform specific)
#ifdef _WIN32
        // Windows arrow key handling
        if (key == 224)
        {
            key = getch();

            switch (key)
            {
            case KEY_UP:
                return movePlayer(world, 0, -1);
            case KEY_DOWN:
                return movePlayer(world, 0, 1);
            case KEY_LEFT:
                return movePlayer(world, -1, 0);
            case KEY_RIGHT:
                return movePlayer(world, 1, 0);
            }
        }
#else
        // Linux arrow key handling
        if (key == 27)
        {                  // ESC
            key = getch(); // should get '['
            if (key == '[')
            {
                key = getch(); // get actual arrow key code

                switch (key)
                {
                case 'A':
                    return movePlayer(world, 0, -1); // Up
                case 'B':
                    return movePlayer(world, 0, 1); // Down
                case 'D':
                    return movePlayer(world, -1, 0); // Left
                case 'C':
                    return movePlayer(world, 1, 0); // Right
                }
            }
        }
#endif

        // Handle WASD keys
        switch (key)
        {
        case 'w':
        case 'W':
            return movePlayer(world, 0, -1);
        case 's':
            return movePlayer(world, 0, 1);
        case 'S':
            displayShipStatus(&world->player);
            return 1;
        case 'a':
        case 'A':
            return movePlayer(world, -1, 0);
        case 'd':
        case 'D':
            return movePlayer(world, 1, 0);
        case 'q':
        case 'Q':
            return -1; // Quit
        case 'u':
        case 'U':
            useJunk(world, 1);
            return 0;
        }
    }
    return 0;
}

void displayIntroduction(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("=== SPACEXPLORER ===\n\n");
        printf("You are an astronaut stranded in the depths of space.\n");
        printf("Your mission is to navigate through the void, collect valuable space junk,\n");
        printf("and find your way back home.\n\n");
    }
    else
    {
        char line[100];
        while (fgets(line, sizeof(line), file))
        {
            printf("%s", line);
        }
        fclose(file);
    }

    printf("\nPress any key to continue...");
    getch();
}

void getPlayerName(char *buffer, int maxLength)
{
    clearScreen();
    printf("Enter your name: ");
    fgets(buffer, maxLength, stdin);

    // Remove newline character if present
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
}

void saveGameScore(const char *filename, const char *playerName, int score, int reason)
{
    FILE *file = fopen(filename, "a");
    if (file == NULL)
        return;

    fprintf(file, "%s %d %d\n", playerName, score, reason);
    fclose(file);
}

void displayLeaderboard(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("No leaderboard data available.\n");
        return;
    }

    printf("\n=== TOP SCORES ===\n");
    printf("%-20s %-10s %s\n", "Player", "Score", "Outcome");
    printf("-------------------------------------------\n");

    char name[50];
    int score, reason;
    int count = 0;

    while (fscanf(file, "%s %d %d", name, &score, &reason) == 3 && count < 10)
    {
        char outcome[20];
        switch (reason)
        {
        case GAME_OVER_WIN:
            strcpy(outcome, "Victory!");
            break;
        case GAME_OVER_FUEL:
            strcpy(outcome, "No fuel");
            break;
        case GAME_OVER_ASTEROID:
            strcpy(outcome, "Asteroid hit");
            break;
        case GAME_OVER_HOME_DESTROYED:
            strcpy(outcome, "Home destroyed");
            break;
        default:
            strcpy(outcome, "Unknown");
        }

        printf("%-20s %-10d %s\n", name, score, outcome);
        count++;
    }

    fclose(file);

    printf("\nPress any key to continue...");
    getch();
}