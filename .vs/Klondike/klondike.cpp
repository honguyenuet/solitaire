#include "card.h"
#include "klondike.h"
#include "sdl_funcs.h"
#include "core.h"
#include "vec2.h"

struct Card* DECK[52];
struct Card* TABLEAUS[7][19];
struct Card* FOUNDATIONS[4][13];
struct Card* DRAWN[52];

unsigned char DECK_SIZE;
unsigned char DRAWN_SIZE;
unsigned char FOUNDATIONS_SIZE[4];
unsigned char TABLEAUS_SIZE[7];

char tableau_picked_up;
char drawn_picked_up;
char foundation_picked_up;
struct Vec2 tableau_picked_up_root;
SDL_Rect picked_up_hitbox;
char which_foundation_picked_up;

SDL_Rect deck_hitbox;
SDL_Rect drawn_hitbox;
SDL_Rect foundation_hitbox[4];

unsigned char __FOUNDATION_CLICKED;

// Sets everything in the deck, tableaus and 
// foundations to the null card
void init_solitaire() {
	for (unsigned char c = 0; c < 52; ++c)
		DECK[c] = &CARDS[0];

	for (unsigned char col = 0; col < 7; ++col)
		for (unsigned char row = 0; row < 19; ++row)
			TABLEAUS[col][row] = &CARDS[0];

	for (unsigned char col = 0; col < 4; ++col)
		for (unsigned char row = 0; row < 13; ++row)
			FOUNDATIONS[col][row] = &CARDS[0];

	for (unsigned char c = 0; c < 52; ++c)
		DRAWN[c] = &CARDS[0];

	for (unsigned char i = 0; i < 4; ++i)
		FOUNDATIONS_SIZE[i] = 0;

	deck_hitbox.x = 20;
	deck_hitbox.y = 20;
	deck_hitbox.w = (int)CARD_SIZE.w;
	deck_hitbox.h = (int)CARD_SIZE.h;

	drawn_hitbox.x = 111;
	drawn_hitbox.y = 20;
	drawn_hitbox.w = deck_hitbox.w;
	drawn_hitbox.h = deck_hitbox.h;

	picked_up_hitbox.w = deck_hitbox.w;
	picked_up_hitbox.h = deck_hitbox.h;

	for (unsigned char f = 0; f < 4; ++f) {
		foundation_hitbox[f].x = 429 + (f * ((CARD_SIZE.w) + 20));
		foundation_hitbox[f].y = 20;
		foundation_hitbox[f].w = deck_hitbox.w;
		foundation_hitbox[f].h = deck_hitbox.h;
	}
}

void shuffle_deck() {
	unsigned char temp;
	for (unsigned char c = 1; c <= 52; ++c) {
		while (1) {
			temp = (rand() % 52);
			// If it is a null card (not set yet)
			if (DECK[temp]->Number == 0) {
				DECK[temp] = &CARDS[c];
				break;
			}
		}
	}
	DECK_SIZE = 52;
	DRAWN_SIZE = 0;
}

void setup_tableaus() {
	for (int t = 0; t < 7; ++t) {
		for (int i = 0; i <= t; ++i) {
			--DECK_SIZE;
			TABLEAUS[t][i] = DECK[DECK_SIZE];
			DECK[DECK_SIZE] = &CARDS[0];
			if (i < t) {
				TABLEAUS[t][i]->Hidden = 1;
			}
		}
	}
}

void update_hitboxes() {
	for (unsigned char t = 0; t < 7; ++t) {
		for (unsigned char c = 0; c < TABLEAUS_SIZE[t]; ++c) {
			TABLEAUS[t][c]->Hitbox.w = CARD_SIZE.w;
			TABLEAUS[t][c]->Hitbox.h = (TABLEAUS_SIZE[t]>=12 ? 15 : 30);
			TABLEAUS[t][c]->Hitbox.x = 111 + (t * (CARD_SIZE.w+15));
			TABLEAUS[t][c]->Hitbox.y = 146 + (c * (TABLEAUS_SIZE[t]>=12 ? 15 : 30));
		}
		if (TABLEAUS_SIZE[t]) {
			TABLEAUS[t][TABLEAUS_SIZE[t]-1]->Hitbox.h = 96;
		}
	}
}

void render_table() {
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_Rect temp = CARD_SIZE;

	for (unsigned char i = 0; i < 4; ++i) {
		if (!(foundation_picked_up && i==which_foundation_picked_up)) {
			if (FOUNDATIONS_SIZE[i]) {
				temp.x = 429 + (i * ((CARD_SIZE.w) + 20));
				temp.y = 20;
				SDL_RenderCopy(renderer, FOUNDATIONS[i][
					FOUNDATIONS_SIZE[i]-1]->Texture, NULL, &temp);
			}
		}
	}

	for (unsigned char i = 0; i < 7; ++i) {
		for (unsigned char c = 0; c < TABLEAUS_SIZE[i]; ++c) {
			if (tableau_picked_up && (tableau_picked_up_root.x == i)
				&& tableau_picked_up_root.y <= c) 
			{
				break;
			} else {
				//temp.x = 111 + (i * (CARD_SIZE.w+15));
				temp.x = TABLEAUS[i][c]->Hitbox.x;
				temp.y = TABLEAUS[i][c]->Hitbox.y;
				//temp.y = 146 + (c * (TABLEAUS_SIZE[i]>=12 ? 15 : 30));
			}
			if (!TABLEAUS[i][c]->Hidden) {
				SDL_RenderCopy(renderer, 
					TABLEAUS[i][c]->Texture, NULL, &temp);
			} else {
				SDL_RenderCopy(renderer, CARD_BACK, NULL, &temp);
			}
		}
	}

	if (tableau_picked_up) {
		int x_diff = mouse_start.x - TABLEAUS[tableau_picked_up_root.x]
			[tableau_picked_up_root.y]->Hitbox.x;
		int y_diff = mouse_start.y - TABLEAUS[tableau_picked_up_root.x]
			[tableau_picked_up_root.y]->Hitbox.y;
		picked_up_hitbox.x = mouse.x - x_diff;
		picked_up_hitbox.y = mouse.y - y_diff;
		for (unsigned char c = tableau_picked_up_root.y; 
			c < TABLEAUS_SIZE[tableau_picked_up_root.x]; ++c)
		{
			temp.x = picked_up_hitbox.x;
			temp.y = picked_up_hitbox.y + ((c-tableau_picked_up_root.y) * 
				(TABLEAUS_SIZE[tableau_picked_up_root.x]>=12 ? 15 : 30));

			if (!TABLEAUS[tableau_picked_up_root.x][c]->Hidden) {
				SDL_RenderCopy(renderer, TABLEAUS[tableau_picked_up_root.x]
					[c]->Texture, NULL, &temp);
			} else {
			SDL_RenderCopy(renderer, CARD_BACK, NULL, &temp);
			}
		}
	}

	if (DECK_SIZE) {
		temp.x = 20;
		temp.y = 20;
		SDL_RenderCopy(renderer, CARD_BACK, NULL, &temp);
	}

	if (drawn_picked_up) {
		if (DRAWN_SIZE > 1) {
			temp.x = 111;
			temp.y = 20;
			SDL_RenderCopy(renderer, DRAWN[DRAWN_SIZE-2]->Texture, NULL, &temp);
		}

		int x_diff = mouse_start.x - drawn_hitbox.x;
		int y_diff = mouse_start.y - drawn_hitbox.y;
		picked_up_hitbox.x = mouse.x - x_diff;
		picked_up_hitbox.y = mouse.y - y_diff;
		SDL_RenderCopy(renderer, DRAWN[DRAWN_SIZE-1]->Texture, NULL, &picked_up_hitbox);
	} else if (DRAWN_SIZE) {
		temp.x = 111;
		temp.y = 20;
		SDL_RenderCopy(renderer, DRAWN[DRAWN_SIZE-1]->Texture, NULL, &temp);
	} else if (foundation_picked_up) {
		if (FOUNDATIONS_SIZE[which_foundation_picked_up] > 1) {
			SDL_RenderCopy(renderer, FOUNDATIONS[which_foundation_picked_up-2][
				FOUNDATIONS_SIZE[which_foundation_picked_up-2]]->Texture, NULL,
				&foundation_hitbox[which_foundation_picked_up-2]);
		}

		int x_diff = mouse_start.x - foundation_hitbox[which_foundation_picked_up].x;
		int y_diff = mouse_start.y - foundation_hitbox[which_foundation_picked_up].y;
		picked_up_hitbox.x = mouse.x - x_diff;
		picked_up_hitbox.y = mouse.y - y_diff;
		SDL_RenderCopy(renderer, FOUNDATIONS[which_foundation_picked_up][
				FOUNDATIONS_SIZE[which_foundation_picked_up-1]]->Texture, NULL, &picked_up_hitbox);
	}
}

void update_sizes() {
	int count = 0;
	for (unsigned char i = 0; i < 4; ++i) {
		count = 0;
		for (unsigned char c = 0; c < 13 && 
			FOUNDATIONS[i][c]!=&CARDS[0]; ++c)
		{
			++count;
		}
		FOUNDATIONS_SIZE[i] = count;
	}

	for (unsigned char t = 0; t < 7; ++t) {
		count = 0;
		for (unsigned char c = 0; c < 19 &&
			TABLEAUS[t][c]!=&CARDS[0]; ++c)
		{
			++count;
		}
		TABLEAUS_SIZE[t] = count;
	}

	count = 0;
	for (unsigned char i = 0; i < 52; ++i) {
		if (DECK[i]==&CARDS[0])
			break;
		++count;
	}
	DECK_SIZE = count;
	
	count = 0;
	for (unsigned char i = 0; i < 52; ++i) {
		if (DRAWN[i]==&CARDS[0])
			break;
		++count;
	}
	DRAWN_SIZE = count;
}

int check_win() {
	int result = 1;
	for (unsigned char i = 0; i < 4; ++i)
		if (FOUNDATIONS_SIZE[i] < 13)
			result = 0;
	return result;
}

enum SECTION_CLICKED check_section_clicked() {
	if (check_collision_point(&deck_hitbox, &mouse)) {
		return __DECK;
	}
	if (DRAWN_SIZE && check_collision_point(&drawn_hitbox, &mouse)) {
		drawn_picked_up = 1;
		return __DRAWN;
	}

	for (unsigned char t = 0; t < 7; ++t) {
		if (TABLEAUS_SIZE[t] && (mouse.x >= TABLEAUS[t][0]->Hitbox.x)
			&& (mouse.x <= TABLEAUS[t][0]->Hitbox.x+TABLEAUS[t][0]->Hitbox.w))
		{
			for (unsigned char c = 0; c < TABLEAUS_SIZE[t]; ++c)
			{
				if (TABLEAUS[t][c]->Hidden || TABLEAUS[t][c] == &CARDS[0]) {
					continue;
				}
				if (c == TABLEAUS_SIZE[t]-1) {
				}
				if (check_collision_point(&TABLEAUS[t][c]->Hitbox, 
					&mouse)) 
				{
					tableau_picked_up = 1;
					tableau_picked_up_root.x = (int)t;
					tableau_picked_up_root.y = (int)c;
					return __TABLEAU;
			
				}
			}
		}
	}

	for (unsigned char f = 0; f < 4; ++f) {
		if (FOUNDATIONS_SIZE[f]) {
			if (check_collision_point(&foundation_hitbox[f], &mouse)) 
			{
				which_foundation_picked_up = f;
				foundation_picked_up = 1;
				return __FOUNDATION;
			}
		}
	}

	return __NULL;
}

void draw_card () {
	if (DECK_SIZE) {
		--DECK_SIZE;
		DRAWN[DRAWN_SIZE] = DECK[DECK_SIZE];
		DECK[DECK_SIZE] = &CARDS[0];
		++DRAWN_SIZE;
	} else if (DRAWN_SIZE) {
		unsigned char D = DRAWN_SIZE;
		for (unsigned char i = 0; i < D; ++i) {
			--DRAWN_SIZE;
			DECK[i] = DRAWN[DRAWN_SIZE];
			DRAWN[DRAWN_SIZE] = &CARDS[0];
			++DECK_SIZE;
		}
	}
}

// Returns 8 if dragged card isn't on a tableau / can't be placed legally
unsigned char check_tableau_dragged_to(struct Card* card) {
	unsigned char tableau = 8;
	for (unsigned char t = 0; t < 7; ++t) {
		if (TABLEAUS_SIZE[t] &&
			check_collision_rect(&picked_up_hitbox, 
				&TABLEAUS[t][TABLEAUS_SIZE[t]-1]->Hitbox) &&
			can_be_placed_onto_tableau(card, TABLEAUS[t][TABLEAUS_SIZE[t]-1])) 
		{
			if (tableau == 8) {
				tableau = t;
			} else {
				tableau = (mouse.x - TABLEAUS[tableau][0]->Hitbox.x 
					< TABLEAUS[t][0]->Hitbox.x - mouse.x ? tableau : t);
				return tableau;
			}
		} else if (!TABLEAUS_SIZE[t]) {
			SDL_Rect *temp = new SDL_Rect;
			temp->x = 111 + (t * (CARD_SIZE.w+15));
			temp->y = 146;
			temp->w = CARD_SIZE.w;
			temp->h = CARD_SIZE.h;
			if (card->Number == 13 && check_collision_rect(&picked_up_hitbox
				, temp)) {
				if (tableau == 8) {
					tableau = t;
				} else {
					tableau = (mouse.x - TABLEAUS[tableau][0]->Hitbox.x 
						< temp->x - mouse.x ? tableau : t);
					return tableau;
				}
			}
		}
	}

	return tableau;
}

unsigned char check_foundation_dragged_to(struct Card* card) {
	if (tableau_picked_up) {
		if (!(TABLEAUS_SIZE[tableau_picked_up_root.x]-1 == tableau_picked_up_root.y)) {
			return 5;
		}
	}

	for (unsigned char f = 0; f < 4; ++f) {
		if (check_collision_rect(&foundation_hitbox[f], &picked_up_hitbox)) {
			if ((!FOUNDATIONS_SIZE[f] && card->Number == 1) || (FOUNDATIONS_SIZE[f] && 
				can_be_placed_onto_foundation(card, FOUNDATIONS[f][FOUNDATIONS_SIZE[f]-1])))
			{
				return f;
			}
		}
	}
	return 5;
}

void append_tableau_onto_tableau(unsigned char tableau, struct Vec2 root) {
	unsigned char s = TABLEAUS_SIZE[root.x];
	for (unsigned char i = root.y; i < s; ++i) {
		TABLEAUS[tableau][TABLEAUS_SIZE[tableau]] = TABLEAUS[root.x][i];
		TABLEAUS[root.x][i] = &CARDS[0];
		++TABLEAUS_SIZE[tableau];
		--TABLEAUS_SIZE[root.x];
	}
}

struct Card* append_card_onto_tableau(unsigned char tableau, struct Card* card) {
	TABLEAUS[tableau][TABLEAUS_SIZE[tableau]] = card;
	++TABLEAUS_SIZE[tableau];
	return &CARDS[0];
}

struct Card* append_card_onto_foundation(unsigned char foundation, struct Card* card) {
	FOUNDATIONS[foundation][FOUNDATIONS_SIZE[foundation]] = card;
	++FOUNDATIONS_SIZE[foundation];
	return &CARDS[0];
}

void uncover_top_hidden_cards() {
	for (unsigned char t = 0; t < 7; ++t) {
		if (TABLEAUS_SIZE[t])
			TABLEAUS[t][TABLEAUS_SIZE[t]-1]->Hidden = 0;
	}
}

void restart_game() {
	init_solitaire();
	shuffle_deck();
	setup_tableaus();
}

void win_animation(Vec2 f_vel[]) {
	for (char f = 0; f < 4; ++f) {
		f_vel[f].y += 1;
		FOUNDATIONS[f][12]->Hitbox.x += f_vel[f].x;
		FOUNDATIONS[f][12]->Hitbox.y += f_vel[f].y;
		if (FOUNDATIONS[f][12]->Hitbox.x < 0) {
			FOUNDATIONS[f][12]->Hitbox.x = 0;
			f_vel[f].x *= -1;
		} else if (FOUNDATIONS[f][12]->Hitbox.y+FOUNDATIONS[f][12]->Hitbox.h 
			> 800) 
		{
			FOUNDATIONS[f][12]->Hitbox.y = 600 - CARD_SIZE.h;
			f_vel[f].y *= -1;
		}
		if (FOUNDATIONS[f][12]->Hitbox.x < 800) {
			SDL_RenderCopy(renderer, FOUNDATIONS[f][12]->Texture, NULL,
				&FOUNDATIONS[f][12]->Hitbox);
		}
	}
	SDL_RenderPresent(renderer);
}