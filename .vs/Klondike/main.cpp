#include <iostream>
#include <ctime>

#include "core.h"
#include "card.h"
#include "klondike.h"

int main(int argc, char** argv) {
	if (init() == 0) {
		std::cout << "SDL: " << SDL_GetError();
		quit();
		return -1;
	}

	define_cards();
	if (load_card_textures() == 0) {
		quit();
		return -1;
	}

	srand(time(0));

	main_loop();
	if (!check_win()) {
		std::cout << "You lose :D" << std::endl;
	}
	delete_card_textures();
	quit();
	return 0;
}