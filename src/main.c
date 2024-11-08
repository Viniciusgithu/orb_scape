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
            if (keyhit()) {
                ch = readch();
                if (ch == 10) break;  // 10 é ENTER
                handleInput(ch);
            }
            
            if (timerTimeOver() == 1) {
                updateGame();
                renderGame();
            }
        }
        
        // Mostra tela de game over e pergunta se quer continuar
        showGameOver();
        
    } while (shouldContinue());
    
    // Finalizar com a limpeza de recursos
    cleanupGame();
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    
    return 0;
}