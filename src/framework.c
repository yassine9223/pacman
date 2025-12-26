#include "framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// pour construire les chemins de fichiers
const char* pathSeparator =
#ifdef _WIN32
        "\\";
#else
        "/";
#endif

#define ZOOM 1.5
#define GRID_CELL_SIZE 14



void init(RendererParameters *params, Textures *textures, int width, int height, const int fps)
{
    // Initialisation du générateur de nombres pseudo-aléatoires
    srand(time(nullptr));

    // Initialisation de SDL3
    if (SDL_Init(0) < 0) { // SDL3 doesn't need subsystem flags here
        printf("Framework -> SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    // Fenêtre
     Uint32 windowFlags = 0;
    if (width == -1 && height == -1) {
        const SDL_DisplayMode *DM = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
        width = DM ? DM->w : 800;
        height = DM ? DM->h : 600;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    else if (width != params->width || height != params->height)
    {
        params->width = width;
        params->height = height;
    }

    // Création de la fenêtre
    params->window = SDL_CreateWindow("Pacman", width, height, windowFlags);
    if (params->window == nullptr) {
        printf("Framework -> SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    SDL_ShowWindow(params->window);

    // Création du moteur de rendu graphique
    params->renderer = SDL_CreateRenderer(params->window, nullptr);
    if (params->renderer == nullptr) {
        printf("Framework -> SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        SDL_DestroyWindow(params->window);
        exit(1);
    }
    // Try to enable vsync (best-effort)
    SDL_SetRenderVSync(params->renderer, 1);
    // Clear once so the first present shows something
    SDL_SetRenderDrawColor(params->renderer, 0, 0, 0, 255);
    SDL_RenderClear(params->renderer);
    SDL_RenderPresent(params->renderer);

    // Textures de chacun des sprites du jeu
    textures->texturePacman = getTexture("pacman.bmp", params);
    textures->textureWall = getTexture("wall.bmp", params);
    textures->textureDot = getTexture("dot.bmp", params);
    textures->textureSuperPacgum = getTexture("super_pacgum.bmp", params);
    textures->textureBlinky = getTexture("blinky.bmp", params);
    textures->textureBlinkyL = getTexture("blinkyL.bmp", params);
    textures->textureBlinkyU = getTexture("blinkyU.bmp", params);
    textures->textureBlinkyD = getTexture("blinkyD.bmp", params);
    textures->texturePinky = getTexture("pinky.bmp", params);
    textures->texturePinkyL = getTexture("pinkyL.bmp", params);
    textures->texturePinkyU = getTexture("pinkyU.bmp", params);
    textures->texturePinkyD = getTexture("pinkyD.bmp", params);
    textures->textureInky = getTexture("inky.bmp", params);
    textures->textureInkyL = getTexture("inkyL.bmp", params);
    textures->textureInkyU = getTexture("inkyU.bmp", params);
    textures->textureInkyD = getTexture("inkyD.bmp", params);
    textures->textureClyde = getTexture("clyde.bmp", params);
    textures->textureClydeL = getTexture("clydeL.bmp", params);
    textures->textureClydeU = getTexture("clydeU.bmp", params);
    textures->textureClydeD = getTexture("clydeD.bmp", params);
    textures->textureBLue = getTexture("blue.bmp", params);

    textures->cellSize = ZOOM * GRID_CELL_SIZE;

    // Délai de rafraichissement
     params->ticks_for_next_frame = (Uint32)(1000u / (Uint32)fps);
     /* Initialize lastTimeScreenUpdate to the current tick count so the first call
         to update() doesn't print a spurious "FPS too high" warning caused by
         comparing against a 0 initial value. */
     params->lastTimeScreenUpdate = SDL_GetTicks();
}

void initWindowed(RendererParameters *params, Textures *textures)
{
    params->width = (int)(WIDTH * ZOOM * GRID_CELL_SIZE);
    params->height = (int)(HEIGHT * ZOOM * GRID_CELL_SIZE);
    init(params, textures, params->width, params->height, FPS);
}


int getInput(void) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            return SDL_EVENT_QUIT;
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            return (int)e.key.key; // SDL3: use 'key' (SDL_Keycode)
        }
    }
    return 0;
}

void update(RendererParameters *params) {
    // On attend le temps nécessaire pour atteindre le taux de FPS
    int nbTicksWait = 0;
    Uint32 now = SDL_GetTicks();
    while (now - params->lastTimeScreenUpdate < params->ticks_for_next_frame) {
        SDL_Delay(1);
        nbTicksWait++;
        now = SDL_GetTicks();
    }

    /* Only print the warning when we actually have a valid previous timestamp.
       This avoids an incorrect warning on the very first frame. */
    if (params->lastTimeScreenUpdate != 0u && nbTicksWait == 0)
        printf("Warning, FPS too high, lagged for %u ms\n", (unsigned)(now - params->lastTimeScreenUpdate));

    // On met à jour l'écran
    SDL_RenderPresent(params->renderer);
    SDL_SetRenderDrawColor(params->renderer, 0, 0, 0, 0);
    SDL_RenderClear(params->renderer);

    // On réarme le temps d'attente pour le prochain rafraichissement
    params->lastTimeScreenUpdate = SDL_GetTicks();
}

void drawSprite(SDL_Texture* texture, const int x, const int y, const float angle, const RendererParameters* params)
{
    // keep existing sizing logic (grid size)
    const SDL_FRect dest = { (float)(x * GRID_CELL_SIZE * ZOOM), (float)(y * GRID_CELL_SIZE * ZOOM),
                       (float)(GRID_CELL_SIZE * ZOOM), (float)(GRID_CELL_SIZE * ZOOM) };
    if (!SDL_RenderTextureRotated(params->renderer, texture, nullptr, &dest, (double)angle, nullptr, SDL_FLIP_NONE))
        printf("DrawSprite -> SDL_RenderTextureRotated failed: %s\n", SDL_GetError());
}

// Match framework.h: void drawSpriteOnGrid(SDL_Texture * texture, int x, int y, float angle, RendererParameters *params);
void drawSpriteOnGrid(SDL_Texture * texture, const int x, const int y, const float angle, const RendererParameters *params) {
    const float cell = GRID_CELL_SIZE * ZOOM;
    const SDL_FRect dest = { ((float)x * cell), ((float)y * cell), cell, cell };
    if (!SDL_RenderTextureRotated(params->renderer, texture, nullptr, &dest, (double)angle, nullptr, SDL_FLIP_NONE))
        printf("DrawSpriteOnGrid -> SDL_RenderTextureRotated failed: %s\n", SDL_GetError());
}

void drawLevel(char** level, const RendererParameters *params, const Textures *textures)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            switch (level[i][j])
            {
            case 'H':
                drawSpriteOnGrid(textures->textureWall, j, i, 0, params);
                break;
            case '.':
                drawSpriteOnGrid(textures->textureDot, j, i, 0, params);
                break;
            case 'G':
                drawSpriteOnGrid(textures->textureSuperPacgum, j, i, 0, params);
                break;
            case ' ':
                break;
            case 'O':
            case 'P':
            case 'I':
            case 'C':
            case 'B':
              // L'affichage de pacman et des fantomes n'est pas gere ici.
            break;

            default:
                printf("ERROR: Wrong element in the given level : %c", level[i][j]);
            }
        }
    }
}


// Création d'une texture de sprite
// --------
// * imgName : nom du fichier de l'image (format BMP avec alpha)
SDL_Texture * getTexture(char* imageName, const RendererParameters *params)
{
    char imagePath[256];
    /* Build path: resources/<sep>/images/<sep>/<imageName> */
    const int written = snprintf(imagePath, sizeof(imagePath), "resources%simages%s%s", pathSeparator, pathSeparator, imageName);
    if (written < 0 || written >= (int)sizeof(imagePath)) {
        printf("GetTexture -> image path too long\n");
        exit(1);
    }
    SDL_Surface * sprite = SDL_LoadBMP(imagePath);

    if (!sprite) {
        printf("SDL_LoadBMP failed: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Texture * texture = SDL_CreateTextureFromSurface(params->renderer, sprite);
    if (!texture) {
        printf("GetTexture -> SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_DestroySurface(sprite);
        exit(1);
    }
    SDL_DestroySurface(sprite);
    return texture;
}

int getRandomNumber() {
    return rand();
}

void cleanUp(const RendererParameters *params, char** level)
{

    if (level)
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            if (level[i]) free(level[i]);
        }
        free(level);
    }

    if (params) {
        if (params->renderer) SDL_DestroyRenderer(params->renderer);
        if (params->window) SDL_DestroyWindow(params->window);
    }
    SDL_Quit();
}
