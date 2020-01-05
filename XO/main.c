#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "plugins.h"

#define WIDTH 900
#define HEIGHT 900


int main(int argc, char* argv[])
{

	srand((int)time(0));
	int mode;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf(SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window *window = SDL_CreateWindow("Ultimate Tic Tac Toe",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (window == NULL) {
		printf(SDL_GetError());
		return EXIT_FAILURE;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		printf(SDL_GetError());
		return EXIT_FAILURE;
	}


	while (1) {

		mode = Menu(window, renderer);

		if (mode) {
			if (Game(window, renderer, mode))
				break;
		}
		else {
			break;
		}

	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

    return EXIT_SUCCESS;

}
