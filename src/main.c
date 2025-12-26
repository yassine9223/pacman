#include "main.h"
#include "firstLevel.h"
#include "framework.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>


int gameLoop(void)
{
    RendererParameters params;
    Textures textures;

    // Initialisation du framework
    initWindowed(&params, &textures);

    SDL_Delay(4000);

    return 0;
}

#ifndef PACMAN_TESTS
int main(void)
{
    gameLoop();
    return 0;
}
#endif
