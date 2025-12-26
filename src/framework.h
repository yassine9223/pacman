/*
 * Special thanks to Brewen DL for SDL2/SDL3 migration
*/

#pragma once

#include <SDL3/SDL.h>

#define FPS 10
#define WIDTH 28
#define HEIGHT 31

typedef struct RendererParameters RendererParameters;
struct RendererParameters
{
    SDL_Window *   window;
    SDL_Renderer * renderer;
    Uint32 ticks_for_next_frame;
    Uint32 lastTimeScreenUpdate;
    int width;
    int height;
};

typedef struct Textures Textures;
struct Textures
{
    SDL_Texture * texturePacman;
    SDL_Texture * textureWall;
    SDL_Texture * textureDot;
    SDL_Texture * textureSuperPacgum;
    SDL_Texture * textureBlinky;
    SDL_Texture * textureBlinkyL;
    SDL_Texture * textureBlinkyU;
    SDL_Texture * textureBlinkyD;
    SDL_Texture * texturePinky;
    SDL_Texture * texturePinkyL;
    SDL_Texture * texturePinkyU;
    SDL_Texture * texturePinkyD;
    SDL_Texture * textureInky;
    SDL_Texture * textureInkyL;
    SDL_Texture * textureInkyU;
    SDL_Texture * textureInkyD;
    SDL_Texture * textureClyde;
    SDL_Texture * textureClydeL;
    SDL_Texture * textureClydeU;
    SDL_Texture * textureClydeD;
    SDL_Texture * textureBLue;
    int cellSize;
};

// Initialisation du framework
//
// Lance l'affichage à la taille du niveau.
// --------
void initWindowed(RendererParameters *params, Textures *textures);

// Entrée du joueur eu clavier
// Renvoie un code clavier SDL3 (https://wiki.libsdl.org/SDL3/SDL_Keycode)
// Renvoie SDL_EVENT_QUIT si l'utilisateur clique sur la croix
int getInput();

// Fonction à appeler à chaque rafraichissement d'image
// Cette fonction effectue une attente active afin de caler l'affichage sur le fps
// -------
// * trackFrameDrop : indique s'il faut afficher un FPS trop important
void update(RendererParameters *params);

// Affichage de sprite
// --------
// * texture : texture du sprite à recopier à l'écran
// * x, y : coordonnées du sprite
// * angle : orientation, en degrés. 0 correspond à la direction nord
void drawSpriteOnGrid(SDL_Texture * texture, int x, int y, float angle, const RendererParameters *params);

// Affichage du fond d'un niveau
// --------
// * level : le niveau sous forme d'un tableau de char
void drawLevel(char** level, const RendererParameters *params, const Textures *textures);

// Création d'une texture de sprite
// --------
// * imgName : nom du fichier de l'image (format BMP avec alpha)
SDL_Texture * getTexture(char* imageName, const RendererParameters *params);

// Renvoie un nombre pseudo-aléatoire (voir srand)
int getRandomNumber();

// Libération des ressources de la SDL
void cleanUp(const RendererParameters *params, char** level);

#ifdef PACMAN_TESTS
// ==== API de test (mock framework) ====
void fw_test_setup(void);
void fw_test_set_inputs(const int *keys, int count);
void fw_test_get_pacman(int *x, int *y);
void fw_test_get_ghost(int index, int *x, int *y);
void fw_test_set_level(char **level);
char **fw_test_get_level(void);
char **fw_test_get_drawn_level(void);
int fw_test_get_draw_count(void);
int fw_test_get_round_count(void);
int fw_test_get_blue_draws(void);
float fw_test_get_pacman_angle(void);
char fw_test_get_ghost_last_dir(int index);
void fw_test_teardown(void);
#endif
