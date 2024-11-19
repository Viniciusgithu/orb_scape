#include "orbScape.h"
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <string.h>

static Player player;
static Enemy enemies[MAX_ENEMIES];
static Level level;
static int gameOver = 0;
static int score = 0;
static int continueGame = 1;
static const char* BORDER = "+";
static const screenColor BORDER_COLOR = YELLOW;


static void customDelay() {
    int i;
    for (i = 0; i < 5; i++) {
        while (!timerTimeOver()) {
            screenUpdate();
        }
    }
}


void showGameOver() {
    screenClear();

    const char* victoryText[] = {
        "Congratulations!",
        "You Completed All Levels!",
        "Your Final Score:",
        "Press 'R' to Retry",
        "Press 'Q' to Quit"
    };

    const char* gameOverText[] = {
        "Game Over!",
        "Your Final Score:",
        "",
        "Press 'R' to Retry",
        "Press 'Q' to Quit"
    };

    // escolha do texto dependendo do resultado final
    const char** selectedText = (level.current == 10) ? victoryText : gameOverText;

    // percorre o array de ponteiro que armazena os textos
    for (int i = 0; i < 5; i++) { 
        screenSetColor((screenColor)(i + RED), BLACK);
        // posição do cursor
        screenGotoxy(MAXX / 2 - strlen(selectedText[i]) / 2, MAXY / 2 - 2 + i);

        printf("%s", selectedText[i]);

        if (i == 2) {
            screenSetColor(YELLOW, BLACK);
            printf("%d", score);
        }

        screenUpdate();
        customDelay();
    }
}


// verificação para continuar ou parar o jogo
int shouldContinue() {
    int ch;
    while (1) {
        if (keyhit()) {
            ch = readch();
            if (ch == 'r' || ch == 'R') return 1;
            if (ch == 'q' || ch == 'Q') return 0;
        }
    }
}

void initializeGame() {
    srand(time(NULL));
    gameOver = 0;
    score = 0;

    player.x = MAXX / 2;
    player.y = MAXY - 2;
    player.sprite = '@';
    player.color = CYAN;

    level.current = 1;
    level.enemyCount = INITIAL_ENEMIES;
    level.baseSpeed = 1;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }

    addEnemiesForLevel();
}

void handleInput(int key) {
    switch (key) {
        case 'a':
            if (player.x > MINX + 1) player.x--;
            break;
        case 'd':
            if (player.x < MAXX - 1) player.x++;
            break;
        case 'w':
            if (player.y > MINY + 1) player.y--;
            break;
        case 's':
            if (player.y < MAXY - 1) player.y++;
            break;
    }
}

void updateGame() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].x += enemies[i].direction * enemies[i].speed;
        if (enemies[i].x >= MAXX - 2 || enemies[i].x <= MINX + 1) {
            enemies[i].direction *= -1;
        }

        if ((player.x == enemies[i].x && player.y == enemies[i].y) ||
            (abs(player.x - enemies[i].x) == 1 && player.y == enemies[i].y) ||
            (abs(player.y - enemies[i].y) == 1 && player.x == enemies[i].x)) {
            gameOver = 1;
            return;
        }
    }

    if (player.y == MINY + 1) {
        score += 100 * level.current;

        if (level.current == 10) {
            gameOver = 1;
            return;
        }

        level.current++;
        level.enemyCount = INITIAL_ENEMIES + (level.current - 1) * 2;
        level.baseSpeed = 1 + (level.current - 1) / 2;

        player.y = MAXY - 2;
        player.x = MAXX / 2;

        addEnemiesForLevel();
    }
}

void renderGame() {
    screenClear();

    screenSetColor(BORDER_COLOR, BLACK);
    for (int x = MINX; x <= MAXX; x++) {
        screenGotoxy(x, MINY);
        printf("%s", BORDER);
        screenGotoxy(x, MAXY);
        printf("%s", BORDER);
    }

    for (int y = MINY; y <= MAXY; y++) {
        screenGotoxy(MINX, y);
        printf("%s", BORDER);
        screenGotoxy(MAXX, y);
        printf("%s", BORDER);
    }

    screenSetColor(player.color, BLACK);
    screenGotoxy(player.x, player.y);
    printf("%c", player.sprite);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        screenSetColor(enemies[i].color, BLACK);
        screenGotoxy(enemies[i].x, enemies[i].y);
        printf("%c", enemies[i].sprite);
    }

    screenSetColor(WHITE, BLACK);
    screenGotoxy(MINX + 2, MINY);
    printf("Level: %d | Score: %d", level.current, score);

    screenUpdate();
}

void addEnemiesForLevel() {
    int enemyCount = 0;
    int enemiesPerRow = level.enemyCount / ENEMY_ROWS;
    if (enemiesPerRow == 0) enemiesPerRow = 1;

    for (int row = 0; row < ENEMY_ROWS && enemyCount < level.enemyCount; row++) {
        for (int col = 0; col < enemiesPerRow && enemyCount < level.enemyCount; col++) {
            int idx = row * enemiesPerRow + col;
            if (idx >= MAX_ENEMIES) break;

            enemies[idx].x = MINX + 2 + (rand() % (MAXX - MINX - 4));
            enemies[idx].y = MINY + 2 + (row * 3);
            enemies[idx].sprite = 'X';
            enemies[idx].color = RED;
            enemies[idx].direction = (rand() % 2) ? 1 : -1;
            enemies[idx].speed = level.baseSpeed + (rand() % 2);
            enemies[idx].active = 1;
            enemyCount++;
        }
    }
}

void cleanupGame() {
}

int isGameOver() {
    return gameOver || (level.current == 10);
}
