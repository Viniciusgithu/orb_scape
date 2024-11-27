#include "orbScape.h"
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <string.h>

Player player;
Enemy* enemies = NULL;
Level level;
int gameOver = 0;
int score = 0;
const char* BORDER = "+";
const screenColor BORDER_COLOR = YELLOW;


void customDelay() { //cria um delay
    int i;
    for (i = 0; i < 5; i++) {
        while (timerTimeOver() == 0) { //enquanto retornar 0, o programa continua
            screenUpdate();
        }
    }
}


void showGameOver() {
    screenClear();

    const char* victoryText[] = { //array de ponteiros para armazenar os textos
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

    // condição para escolha do texto dependendo se o jogador ganhou ou não
    const char** selectedText = (level.current == 10) ? victoryText : gameOverText;

    // percorre o array de ponteiro que armazena os textos
    for (int i = 0; i < 5; i++) { 
        screenSetColor((screenColor)(i + RED), BLACK); //percorre o array de ponteiros e define as cores do enum
        // posição do texto
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
    srand(time(NULL)); //gera numeros aleatórios de acordo com cada execução
    gameOver = 0;
    score = 0;

    player.x = MAXX / 2;
    player.y = MAXY - 2;
    player.sprite = '@';
    player.color = CYAN;
    
    level.current = 1;
    level.enemyCount = INITIAL_ENEMIES;
    level.baseSpeed = 1;


    enemies = malloc(level.enemyCount * sizeof(Enemy)); //aloca a quantidade inicial de inimigos
    if (enemies == NULL) {
        printf("Erro ao alocar memória para os inimigos.\n");
        exit(1);
    }


    addEnemiesForLevel();
}

void handleInput(int key) {
    switch (key) {
        case 'a':
            if (player.x > MINX + 1) player.x--; //anda para esquerda
            break;
        case 'd':
            if (player.x < MAXX - 1) player.x++; //anda para direita
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
        enemies[i].x += enemies[i].direction * enemies[i].speed; //calcula a movimentação do inimigo
        //verifica a colisão com as bordas
        if (enemies[i].x >= MAXX - 2 || enemies[i].x <= MINX + 1) {
            enemies[i].direction *= -1;
        }
        //colisão do player com o inimigo
        if ((player.x == enemies[i].x && player.y == enemies[i].y) ||
            (abs(player.x - enemies[i].x) == 1 && player.y == enemies[i].y) || //verifica se o player encosta no inimigo horizontalmente
            (abs(player.y - enemies[i].y) == 1 && player.x == enemies[i].x)) { //verifica verticalmente
            gameOver = 1;
            return;
        }
    }

    if (player.y == MINY + 1) { //verifica se chegou ao topo e verifica com o MINY porque o minimo começa em cima
        score += 100 * level.current;

        if (level.current == 10) {
            gameOver = 1;
            return;
        }

        level.current++;
        level.enemyCount += 1;
        level.baseSpeed = 1 + (level.current - 1) / 2;

        enemies = realloc(enemies, level.enemyCount * sizeof(Enemy));
        if (enemies == NULL) {
            printf("Erro ao realocar memória\n");
            exit(1); //realoca dinamicamente a quantidade dos inimigos a cada nível
        }

        player.y = MAXY - 2; //posiciona o jogador
        player.x = MAXX / 2;

        addEnemiesForLevel();
    }
}

void renderGame() {
    screenClear();

    screenSetColor(BORDER_COLOR, BLACK);
    for (int x = MINX; x <= MAXX; x++) { //percorre da esquerda para direita, do 1 ao 80
        screenGotoxy(x, MINY);
        printf("%s", BORDER);
        screenGotoxy(x, MAXY);
        printf("%s", BORDER);
    }

    for (int y = MINY; y <= MAXY; y++) { //percorre de cima para baixo, do 1 ao 24
        screenGotoxy(MINX, y);
        printf("%s", BORDER);
        screenGotoxy(MAXX, y);
        printf("%s", BORDER);
    }

    screenSetColor(player.color, BLACK);
    screenGotoxy(player.x, player.y);
    printf("%c", player.sprite);

    for (int i = 0; i < MAX_ENEMIES; i++) { //definição da cor, posição e skin dos inimigos
        screenSetColor(enemies[i].color, BLACK);
        screenGotoxy(enemies[i].x, enemies[i].y);
        printf("%c", enemies[i].sprite);
    }

    screenSetColor(WHITE, BLACK); //definição da cor e posição do texto do level e score
    screenGotoxy(MINX + 2, MINY);
    printf("Level: %d | Score: %d", level.current, score);

    screenUpdate();
    
}


void addEnemiesForLevel() {
    for (int i = 0; i < level.enemyCount; i++) {

        enemies[i].x = MINX + 2 + (rand() % (MAXX - MINX - 4)); // posição horizontal
        enemies[i].y = MINY + 2 + (i * 2); // distribuição vertical, cada inimigo em uma nova linha
        enemies[i].sprite = 'X'; // caractere que representa o inimigo
        enemies[i].color = RED; // cor do inimigo
        enemies[i].direction = (rand() % 2) ? 1 : -1; // direção aleatória (esquerda ou direita)
        enemies[i].speed = level.baseSpeed + (rand() % 2); // velocidade com base no nível
    }
}


void cleanupGame() {
    if (enemies != NULL) { //libera a memória
        free(enemies);
        enemies = NULL;
    }
}

int isGameOver() {
    return gameOver || (level.current == 10); //verifica se o jogo terminou
}
