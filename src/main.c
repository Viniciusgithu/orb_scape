#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "orbScape.h"

int main() {
    int ch = 0;
    
    screenInit(1);
    keyboardInit();
    timerInit(50);
    
    do {
        initializeGame();
        
        while (!isGameOver()) {
            if (keyhit()) { //verifica se alguma tecla foi pressionada
                ch = readch(); //le a tecla pressionada
                if (ch == 10) break;  // 10 é ENTER
                handleInput(ch); //processar e ler a movimentação do jogador
            }
            
            if (timerTimeOver() == 1) {  //se o tempo tiver esgotado
                updateGame();
                renderGame();
            }
        }
        
        showGameOver();
        
    } while (shouldContinue()); //verifica se o jogador quer reiniciar o jogo ou sair
    
    cleanupGame();
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    
    return 0;
}