#ifndef _HELPER_H_
#define _HELPER_H_
#include <stdlib.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Window   extern *Window;
SDL_Renderer extern *Renderer;

void init (void);
void exit_(void);

#endif
