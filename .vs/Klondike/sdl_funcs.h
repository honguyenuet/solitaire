#ifndef SDL_FUNCS_H
#define SDL_FUNCS_H

#include <SDL.h>

SDL_Texture* load_texture(const char* filename, int* success);
void draw_texture(SDL_Texture* texture, int x, int y, int w, int h);

#endif // SDL_FUNCS_H
