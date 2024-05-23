#include <SDL.h>

#include "sdl_funcs.h"
#include "card.h"
#include "core.h"

struct Card CARDS[53];
SDL_Texture* CARD_TEXTURES[53];
SDL_Texture* CARD_BACK;
SDL_Rect     CARD_SIZE;

void define_cards() {
	CARDS[0].Number = 0;
	// 1 -13 = CLUB; 14 - 26 = DIAMOND 
	// 27-39 = HEART; 40-52 SPADE
	for (unsigned char num = 1; num < 14; ++num) {
		for (unsigned char offset = 0; offset <= 39; offset+=13) {
			CARDS[num+offset].Number = num;
		}
	}

	for (char i=1 ; i<=13; ++i) CARDS[i].Suit = CLUB;
	for (char i=14; i<=26; ++i) CARDS[i].Suit = DIAMOND;
	for (char i=27; i<=39; ++i) CARDS[i].Suit = HEART;
	for (char i=40; i<=52; ++i) CARDS[i].Suit = SPADE;

	for (char i = 1; i <= 52; ++i) {
		if ((CARDS[i].Suit == DIAMOND) || (CARDS[i].Suit == HEART)) {
			CARDS[i].Color = RED;
		} else {
			CARDS[i].Color = BLACK;
		}
	}
}

int can_be_placed_onto_tableau(struct Card* a, struct Card* b) {
	return ((a->Color != b->Color) && (a->Number == b->Number - 1)) ?
		1 : 0;
}

int can_be_placed_onto_foundation(struct Card* a, struct Card* b) {
	return (a->Suit == b->Suit) && (a->Number == b->Number + 1) ?
		1 : 0;
}

void map_card_to_filename(struct Card card, char dest[]) {
	switch (card.Suit) {
	case CLUB:
		dest[4] = 'c';
		break;
	case DIAMOND:
		dest[4] = 'd';
		break;
	case HEART:
		dest[4] = 'h';
		break;
	case SPADE:
		dest[4] = 's';
		break;
	}

	// primitve number to string
	unsigned char temp = card.Number;
	if (card.Number >= 10) {
		dest[5] = '1';
		temp -= 10;
	} else {
		dest[5] = '0';
	}
	dest[6] = (temp + '0');
}

int load_card_textures() {
	char filename[] = "bmp/000.bmp";
	int success = 1, temp;

	for (unsigned char i = 1; i <= 52; ++i) {
		map_card_to_filename(CARDS[i], filename);
		CARD_TEXTURES[i] = load_texture(filename, &temp);
		CARDS[i].Texture = CARD_TEXTURES[i];
		if (temp == 0) success = 0;
	}
	SDL_QueryTexture(CARD_TEXTURES[1], NULL, NULL,
		&CARD_SIZE.w, &CARD_SIZE.h);

	CARD_BACK = load_texture("bmp/back.bmp", &temp);
	if (temp == 0) success = 0;

	background = load_texture("bmp/background.bmp", &temp);
	if (temp == 0) success = 0;
	return success;
}

void delete_card_textures() {
	for (size_t i = 1; i <= 52; ++i)
		SDL_DestroyTexture(CARD_TEXTURES[i]);
	SDL_DestroyTexture(CARD_BACK);
}
