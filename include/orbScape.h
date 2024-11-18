#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define MAX_ENEMIES 10
#define INITIAL_ENEMIES 3
#define ENEMY_ROWS 5

// Estrutura para o jogador
typedef struct {
    int x, y;
    char sprite;
    screenColor color;
} Player;

// Estrutura para o inimigo
typedef struct {
    int x, y;
    char sprite;
    screenColor color;
    int direction;
    int speed;
    int active;
} Enemy;

// Estrutura para o nível
typedef struct {
    int current;
    int enemyCount;
    int baseSpeed;
} Level;

typedef struct {
    char name[50];
    int score;
} PlayerScore;

// Funções do jogo
void initializeGame();
void updateGame();
void renderGame();
void handleInput(int key);
void cleanupGame();
int isGameOver();
int shouldContinue();
void showGameOver();
void showVictory();
void resetGame();
void addEnemiesForLevel();

#endif