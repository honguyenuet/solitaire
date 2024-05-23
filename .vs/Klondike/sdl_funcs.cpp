#include "core.h"
#include "sdl_funcs.h"

SDL_Texture* load_texture(const char* filename, int* success) {
	*success = 1;

	SDL_Surface* s; SDL_Texture* t;
	s = SDL_LoadBMP(filename);

	if (s == NULL) {
		std::cout << "SDL: " <<  SDL_GetError();
		*success = 0;
	}

	t = SDL_CreateTextureFromSurface(renderer, s);
	if (t == NULL) {
		std::cout << "SDL: ", SDL_GetError();
		*success = 0;
	}

	return t;
}

void draw_texture(SDL_Texture* texture, int x, int y, int w, int h) {
	SDL_Rect rect = {x,y,w,h};
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}
