#include "Rendering.h"

#include <SDL.h>

namespace
{
	constexpr int WINDOW_WIDTH = 1280;
	constexpr int WINDOW_HEIGHT = 720;

	bool appRunning = true;
}

void RunWindow()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL Failed to start: %s", SDL_GetError());
		appRunning = false;
		return;
	}

	SDL_Window* window = SDL_CreateWindow("NEON RUSH", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!window)
	{
		SDL_Log("SDL Crashed at window creation: %s", SDL_GetError());
		SDL_Quit();
		appRunning = false;
		return;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer)
	{
		SDL_Log("SDL Crashed at renderer creation: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		appRunning = false;
		return;
	}

	while (appRunning)
	{
		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}