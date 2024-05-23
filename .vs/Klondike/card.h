#ifndef CARD_H
#define CARD_H

#include <SDL.h>

enum SUIT {null, CLUB, DIAMOND, HEART, SPADE};
enum COLOR {BLACK=10, RED=11};

struct Card {
	enum SUIT Suit;
	enum COLOR Color;
	unsigned char Number; // 11-13 JACK-KING
	char Hidden;
	SDL_Texture* Texture;
	SDL_Rect Hitbox;
};

extern struct Card CARDS[53]; // 1 extra card to act as a null card.

// 53 aswell so cards index can be mapped directly to its texture.
extern SDL_Texture* CARD_TEXTURES[53];
extern SDL_Texture* CARD_BACK;
extern SDL_Rect     CARD_SIZE;

void define_cards();

int can_be_placed_onto_tableau(struct Card* a, struct Card* b);
int can_be_placed_onto_foundation(struct Card* a, struct Card* b);

// Maps card to the filename of its bmp
void map_card_to_filename(struct Card card, char dest[]);

int load_card_textures();
void delete_card_textures();

#endif // CARD_H
