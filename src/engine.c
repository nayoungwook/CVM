#include "includes/engine.h"

void initialize_engine() {
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_EVERYTHING);
}

SDL_Window* create_window(const wchar_t* title, int width, int height) {
	SDL_Window* result = SDL_CreateWindow(_wcsdup(title), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

	if (!result) {
		// error handle
		SDL_Quit();
		return 1;
	}

	return result;
}