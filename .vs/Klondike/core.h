#ifndef CORE_H
#define CORE_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

#include "vec2.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Event event;
extern const int WIDTH, HEIGHT;

extern struct Vec2 mouse;
extern struct Vec2 mouse_start;
extern SDL_Texture* background;

int init();
void quit();
void main_loop();
int check_collision_point(SDL_Rect* hitbox, struct Vec2* point);
int check_collision_rect(SDL_Rect* a, SDL_Rect* b);

#endif // CORE_H