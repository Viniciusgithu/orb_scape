/**
 * timer.c
 * Created on Aug, 23th 2023
 * Author: Tiago Barros
 * Based on "From C to C++ course - 2002"
*/

#include "timer.h"
#include <sys/time.h>
#include <stdio.h>

static struct timeval timer, now;
static int delay = -1;

void timerInit(int valueMilliSec)
{
    delay = valueMilliSec;
    gettimeofday(&timer, NULL);
}

void timerDestroy()
{
    delay = -1;
}

void timerUpdateTimer(int valueMilliSec)
{
    delay = valueMilliSec;
    gettimeofday(&timer, NULL);
}

int getTimeDiff() //calcula a diferença do tempo atual com o momento do timer
{
    gettimeofday(&now, NULL);
    long diff = (((now.tv_sec - timer.tv_sec) * 1000000) + now.tv_usec - timer.tv_usec)/1000;
    return (int) diff;
}

int  timerTimeOver() //calcula a diferença entre o momento atual e o momento que o temporizador foi inicializado
{
    int ret = 0;

    if (getTimeDiff() > delay)
    {
        ret = 1;
        gettimeofday(&timer, NULL);
    }

    return ret;
}

void timerPrint()
{
    printf("Timer:  %d", getTimeDiff());
}