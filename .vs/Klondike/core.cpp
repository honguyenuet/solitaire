#include "vec2.h"
#include "core.h"
#include "klondike.h"

SDL_Renderer* renderer;
SDL_Window* window;
SDL_Event event;
Mix_Chunk *draw1, *drawRes;
const int WIDTH = 800, HEIGHT = 600;
int score = 200;

struct Vec2 mouse;
struct Vec2 mouse_start;
SDL_Texture* background;

int init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 0;
	else
		std::cout << "SDL: Video initalized.\n";

	window = SDL_CreateWindow("Solitaire", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) 
		return 0;
	else
		std::cout << "SDL: Window created.\n";

	SDL_Surface* icon = SDL_LoadBMP("bmp/logo.bmp");
	if (icon != NULL) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	} else {
		std::cout << "Can't load logo!\n";
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL) {
			return 0;
		} else {
			std::cout <<"SDL: Software renderer created.\n";
		}
	} else {
		std::cout << "SDL: Accelerated renderer created.\n";
	}
	if (Mix_Init(0) < 0) {
		return 0;
	}
	else {
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
		draw1 = Mix_LoadWAV("wav/place1.wav");
		drawRes = Mix_LoadWAV("wav/place2.wav");
		std::cout << "SDL: Mixer initalized.\n";
	}
	std::cout << std::endl;
	return 1;
}

void quit() {
	SDL_DestroyTexture(background);
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

int check_collision_point(SDL_Rect* hitbox, struct Vec2* point) {
	return (hitbox->x <= point->x) && (hitbox->y <= point->y) 
		&& (hitbox->x + hitbox->w >= point->x) 
		&& (hitbox->y + hitbox->h >= point->y) ? 1 : 0;
}

int check_collision_rect(SDL_Rect* a, SDL_Rect* b) {
	return (abs(a->x - b->x) * 2 < (a->w + b->w)) && 
		(abs(a->y - b->y) * 2 < (a->h + b->h)) ? 1 : 0;
}

void main_loop() {
	char playing = 1;
	char mouse_left_down = 0;
	char draw_frame = 1;
	char won = 0;
	struct Vec2 f_vel[4];

	enum SECTION_CLICKED s_clicked;

	init_solitaire();
	shuffle_deck();

	setup_tableaus();
	update_sizes();
	update_hitboxes();

	while (playing) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				playing = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&mouse_start.x, 
						&mouse_start.y);
					mouse = mouse_start;
					mouse_left_down = 1;
					update_sizes();
					update_hitboxes();
					s_clicked = check_section_clicked();
					switch (s_clicked) {
					case __DECK:
						if (DECK_SIZE > 0)
							Mix_PlayChannel(-1, draw1, 0);
						else
							Mix_PlayChannel(-1, drawRes, 0);
						draw_card();
						draw_frame = 1;
						break;
					case __TABLEAU:
						if (event.button.clicks == 2) {
							Card* card = TABLEAUS[tableau_picked_up_root.x][tableau_picked_up_root.y];
							for (unsigned char f = 0; f < 4; f++) {
								if ((!FOUNDATIONS_SIZE[f] && card->Number == 1) || (FOUNDATIONS_SIZE[f] && can_be_placed_onto_foundation(card, FOUNDATIONS[f][FOUNDATIONS_SIZE[f] - 1]))) {
									append_card_onto_foundation(f, card);
									--TABLEAUS_SIZE[tableau_picked_up_root.x];
									TABLEAUS[tableau_picked_up_root.x][tableau_picked_up_root.y] = &CARDS[0];
									score--;
									break;
								}
							}
						}
						break;
					case __DRAWN:
						if (event.button.clicks == 2) {
							Card* card = DRAWN[DRAWN_SIZE - 1];
							for (unsigned char f = 0; f < 4; f++) {
								if ((!FOUNDATIONS_SIZE[f] && card->Number == 1) || (FOUNDATIONS_SIZE[f] && can_be_placed_onto_foundation(card, FOUNDATIONS[f][FOUNDATIONS_SIZE[f] - 1]))) {
									append_card_onto_foundation(f, card);
									--DRAWN_SIZE;
									DRAWN[DRAWN_SIZE] = &CARDS[0];
									score--;
									break;
								}
							}
						}
						break;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse_left_down = 0;
					if (tableau_picked_up) {
						unsigned char I = 
							check_tableau_dragged_to(
								TABLEAUS[
								tableau_picked_up_root.x][
								tableau_picked_up_root.y]
							);
						if (I != 8) {
							append_tableau_onto_tableau(I,
							tableau_picked_up_root);
							score--;
						} else {
							I = check_foundation_dragged_to(
								TABLEAUS[
								tableau_picked_up_root.x][
								tableau_picked_up_root.y]);
							if (I != 5) {
								TABLEAUS[
								tableau_picked_up_root.x]
								[tableau_picked_up_root.y] =
								append_card_onto_foundation(
									I, TABLEAUS[
								tableau_picked_up_root.x][
								tableau_picked_up_root.y]);
								if (tableau_picked_up_root.y>1) {}
								--TABLEAUS_SIZE
								[tableau_picked_up_root.x];
								score--;
								update_sizes();
							}

							tableau_picked_up = 0;
						}
					} else if (drawn_picked_up) {
						unsigned char I =
							check_tableau_dragged_to(
								DRAWN[DRAWN_SIZE-1]);
						if (I != 8) {
							DRAWN[DRAWN_SIZE-1] = 
							append_card_onto_tableau(I, 
								DRAWN[DRAWN_SIZE-1]);
							--DRAWN_SIZE;
							score--;
						} else {
							I = check_foundation_dragged_to(
								DRAWN[DRAWN_SIZE-1]);
							if (I != 5) {
								DRAWN[DRAWN_SIZE-1] = 
								append_card_onto_foundation(
									I, DRAWN[DRAWN_SIZE-1]);
								--DRAWN_SIZE;
								score--;
							}
						}
						drawn_picked_up = 0;
					} else if (foundation_picked_up) {
						unsigned char I = check_tableau_dragged_to(
							FOUNDATIONS[which_foundation_picked_up]
							[FOUNDATIONS_SIZE[which_foundation_picked_up]-1]);
						if (I != 8) {
							FOUNDATIONS[which_foundation_picked_up]
							[FOUNDATIONS_SIZE[which_foundation_picked_up]-1]
							= append_card_onto_tableau(
								I, FOUNDATIONS[which_foundation_picked_up]
							[FOUNDATIONS_SIZE[which_foundation_picked_up]-1]);

							--FOUNDATIONS_SIZE[which_foundation_picked_up];
							score--;
						}
						foundation_picked_up = 0;
					}

					uncover_top_hidden_cards();
					update_sizes();
					update_hitboxes();
					draw_frame = 1;

					if (check_win()) {
						won = 1;
						for (unsigned char f = 0; f < 4; ++f) {
							FOUNDATIONS[f][12]->Hitbox = foundation_hitbox[f];
							f_vel[f].x = -5;
							f_vel[f].y = 0;
						}
						std::cout << "Your score is: " << score << std::endl;
					}
				}	
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					draw_frame = 1;
				}
				break;
			}
		}

		if (mouse_left_down) {
			SDL_GetMouseState(&mouse.x, &mouse.y);
			draw_frame = 1;
		}

		if (won) {
			win_animation(f_vel);
			SDL_RenderPresent(renderer);
			draw_frame = 0;
		} if (draw_frame) {
			render_table();
			SDL_RenderPresent(renderer);
			draw_frame = 0;
		}
	}
}