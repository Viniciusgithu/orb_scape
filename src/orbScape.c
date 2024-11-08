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

// Delay usando o timerTimeOver
static void customDelay() {

    int i;

    for(i = 0; i < 5; i++) {  // Esperar por 5 ciclos do timer
        // Enquanto o tempo não acabar, atualiza a tela 
        while(!timerTimeOver()) {
            screenUpdate();
        }

    }
}

// static void customDelay() {
//     // Define um FPS constante para evitar piscadas
//     const int TARGET_FPS = 30;
//     const int FRAME_TIME = 1000 / TARGET_FPS; // tempo em ms por frame
    
//     timerInit(FRAME_TIME);
    
//     // Espera até o próximo frame
//     while(!timerTimeOver()) {
//         // Não atualiza a tela aqui para evitar flickering
//     }
//     // Atualiza a tela apenas uma vez por frame
//     screenUpdate();
// }

// Tela de Game Over
void showGameOver() {
    // Limpa a tela
    screenClear();
    
    // Animação de Game Over
    const char* gameOverText[] = {
        "Game Over!",
        "Your Final Score:",
        "",
        "Press 'R' to Retry",
        "Press 'Q' to Quit"
    };

    // Percorre o texto de Game Over
    for (int i = 0; i < sizeof(gameOverText)/sizeof(gameOverText[0]); i++) {
        // Define a cor do texto
        screenSetColor((screenColor)(i + RED), BLACK);
        // Posiciona o cursor no centro da tela
        screenGotoxy(MAXX/2 - strlen(gameOverText[i])/2, MAXY/2 - 2 + i); 
        // MAXX: max screen horizontal pos
        // MAXY: max screen vertical pos

        // Imprime o texto
        printf("%s", gameOverText[i]);
        

        // Se for a linha do score, imprime o score
        if (i == 2) {
            // Define a cor do score
            screenSetColor(YELLOW, BLACK);
            printf("%d", score);
        }
        
        // Atualiza a tela e delay
        screenUpdate();
        customDelay();
    }
}


// Verificar se o jogo deve continuar
int shouldContinue() {

    int ch;

    // Enquanto não houver input, atualiza a tela
    while (1) {
        // Se houver input, verifica se é para continuar ou não
        if (keyhit()) {
            ch = readch();
            if (ch == 'r' || ch == 'R') return 1;
            if (ch == 'q' || ch == 'Q') return 0;
        }
        if(timerTimeOver()) {
            screenUpdate();
        }
    }
}

// Inicializar o game
void initializeGame() {

    // Inicializa gerador de números aleatórios
    srand(time(NULL));
    
    // Reseta o estado do jogo
    gameOver = 0;
    score = 0;
    
    // Inicializa o jogador
    // Posiciona o jogador no centro da tela
    player.x = MAXX / 2;
    // Posiciona o jogador na parte inferior da tela  
    player.y = MAXY - 2;
    // Define o sprite do jogador
    player.sprite = '@';
    // Define a cor do jogador
    player.color = CYAN;


    
    // Inicializa o nível
    level.current = 1;
    // Define a quantidade de inimigos iniciais
    level.enemyCount = INITIAL_ENEMIES;
    // Define a velocidade base dos inimigos
    level.baseSpeed = 1;


    
    // Limpa todos os inimigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }
    
    // Adiciona inimigos iniciais
    addEnemiesForLevel();
}


// Movimentar o jogador
void handleInput(int key) {


    switch(key) {

        // Move o jogador para a esquerda
        case 'a':
            if (player.x > MINX + 1) player.x--;
            break;
        // Move o jogador para a direita    
        case 'd':
            if (player.x < MAXX - 1) player.x++;
            break;
        // Move o jogador para cima
        case 'w':
            if (player.y > MINY + 1) player.y--;
            break;
        case 's':
        // Move o jogador para baixo
            if (player.y < MAXY - 1) player.y++;
            break;
    }
}


// Atualiza o estado do jogo
void updateGame() {

    // Atualiza posição dos inimigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
          // Caso o inimigo não esteja ativo, pula para o próximo
        // if (!enemies[i].active) continue;
        
        // Move o inimigo
        enemies[i].x += enemies[i].direction * enemies[i].speed;
        
        // Verifica colisão com as paredes
        if (enemies[i].x >= MAXX - 2 || enemies[i].x <= MINX + 1) {
            enemies[i].direction *= -1;
        }
        
        // Verifica colisão com o jogador
        // if (player.x == enemies[i].x && player.y == enemies[i].y) {
        //     gameOver = 1;
        //     return;
        // }

        if ((player.x == enemies[i].x && player.y == enemies[i].y) ||
            (abs(player.x - enemies[i].x) == 1 && player.y == enemies[i].y) || 
            (abs(player.y - enemies[i].y) == 1 && player.x == enemies[i].x)) {
             gameOver = 1;  // Acaba o jogo se houver colisão
             return;
}
    }
    
    // Verifica se o jogador chegou ao topo
    if (player.y == MINY + 1) {
        score += 100 * level.current;
        level.current++;
        level.enemyCount = INITIAL_ENEMIES + (level.current - 1) * 2;
        level.baseSpeed = 1 + (level.current - 1) / 2;
        
        // Reposiciona o jogador
        player.y = MAXY - 2;
        
        // Adiciona novos inimigos
        addEnemiesForLevel();
    }
}


// Renderiza o estado do jogo
void renderGame() {
    // Limpa a tela anterior
    screenClear();
    
    // Desenhar as bordas
    screenSetColor(BORDER_COLOR, BLACK);


    // Percorre as bordas superior e inferior
    for (int x = MINX; x <= MAXX; x++) {
        // Posiciona o cursor na posição da borda
        screenGotoxy(x, MINY); // min screen vertical pos
        // Imprime a borda
        printf("%s", BORDER);
        // Posiciona o cursor na posição da borda
        screenGotoxy(x, MAXY); // max screen vertical pos
        // Imprime a borda
        printf("%s", BORDER);
    }

    // Percorre as bordas laterais
    for (int y = MINY; y <= MAXY; y++) {
        screenGotoxy(MINX, y);
        printf("%s", BORDER);
        screenGotoxy(MAXX, y);
        printf("%s", BORDER);
    }
    
    // Desenhar o jogador
    screenSetColor(player.color, BLACK);
    // Posiciona o cursor na posição do jogador
    screenGotoxy(player.x, player.y);
    // Imprime o sprite do jogador
    printf("%c", player.sprite);
    
    // Desenhar os inimigos iterando sobre ele
    for (int i = 0; i < MAX_ENEMIES; i++) {
        // Caso o inimigo não esteja ativo, pula para o próximo
        if (!enemies[i].active) continue;
        
        // Desenha o inimigo
        screenSetColor(enemies[i].color, BLACK);
        // Posiciona o cursor na posição do inimigo
        screenGotoxy(enemies[i].x, enemies[i].y);
        // Imprime o sprite do inimigo
        printf("%c", enemies[i].sprite);
    }
    
    // Desenha informações do jogo
    screenSetColor(WHITE, BLACK);
    screenGotoxy(MINX + 2, MINY);
    printf("Level: %d | Score: %d", level.current, score);
    
    // Atualizar a tela
    screenUpdate();
}


// Adiciona inimigos para o nível
void addEnemiesForLevel() {

    // Contador de inimigos
    int enemyCount = 0;

    // Definir a quantidade de inimigos por linha
    int enemiesPerRow = level.enemyCount / ENEMY_ROWS;

    // Caso não haja inimigos suficientes para preencher todas as linhas
    if (enemiesPerRow == 0) enemiesPerRow = 1;
    
    // Adiciona inimigos para cada linha
    for (int row = 0; row < ENEMY_ROWS && enemyCount < level.enemyCount; row++) {
        // Adiciona inimigos para cada coluna
        for (int i = 0; i < enemiesPerRow && enemyCount < level.enemyCount; i++) {
            // Calcula o índice do inimigo
            int idx = row * enemiesPerRow + i;
            // Caso o índice seja maior que o máximo de inimigos, sai do loop
            if (idx >= MAX_ENEMIES) break;
            

            // Define a posição do inimigo
            enemies[idx].x = MINX + 2 + (rand() % (MAXX - MINX - 4));

            // Define a posição do inimigo
            enemies[idx].y = MINY + 2 + (row * 3);

            // Define o sprite do inimigo
            enemies[idx].sprite = 'X';

            // Define a cor do inimigo
            enemies[idx].color = RED;

            // Define a direção do inimigo
            enemies[idx].direction = (rand() % 2) ? 1 : -1;

            // Define a velocidade do inimigo
            enemies[idx].speed = level.baseSpeed + (rand() % 2);

            // Define o inimigo como ativo
            enemies[idx].active = 1;

            // Incrementa o contador de inimigos            
            enemyCount++;
        }
    }
}


// Limpa recursos do jogo
void cleanupGame() {
    // Sem recursos p/ limpar
}


// Verifica se o jogo acabou
int isGameOver() {
    return gameOver;
}