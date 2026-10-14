#include "Rendering.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// ---------- CONSTANTS ----------
const int screenWidth = 1280;
const int screenHeight = 720;

// -------- Constansts -------------
namespace
{
	constexpr int WINDOW_WIDTH = 1280;
	constexpr int WINDOW_HEIGHT = 720;

	bool appRunning = true;
}

void RunWindow()
{
	// ------------ Error Check -----------
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL Failed to start: %s", SDL_GetError());
		appRunning = false;
		return;
	}

	// ------------ Error Check -----------
	SDL_Window* window = SDL_CreateWindow("NEON RUSH", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!window)
	{
		SDL_Log("SDL Crashed at window creation: %s", SDL_GetError());
		SDL_Quit();
		appRunning = false;
		return;
	}

	// ------------ Error Check -----------
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer)
	{
		SDL_Log("SDL Crashed at renderer creation: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		appRunning = false;
		return;
	}

	// ---------- Render -----------
	while (appRunning)
	{
		// ---------- UPDATE ----------
		float deltaTime = GetFrameTime();
		// ---------- MOVEMENT ----------

		// ----- RIGHT -----
		if (IsKeyDown(KEY_RIGHT)) ballPos.x += speed;
		else if (IsKeyDown(KEY_D)) ballPos.x += speed;

		// ----- LEFT -----
		if (IsKeyDown(KEY_LEFT)) ballPos.x -= speed;
		else if (IsKeyDown(KEY_A)) ballPos.x -= speed;

		// ----- UP -----
		if (IsKeyDown(KEY_UP)) ballPos.y -= speed;
		else if (IsKeyDown(KEY_W)) ballPos.y -= speed;

		// ----- DOWN -----
		if (IsKeyDown(KEY_DOWN)) ballPos.y += speed;
		else if (IsKeyDown(KEY_S)) ballPos.y += speed;

		ballPos.x = Clamp(ballPos.x, radius, screenWidth - radius);
		ballPos.y = Clamp(ballPos.y, radius, screenHeight - radius);


		// ---------- AI MOVEMENT ----------

		agent->MoveToPlayer(ballPos, 100.0f, deltaTime);
		
		// ---------- RENDER ----------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawCircleV(ballPos, radius, MAROON);

		agent->Draw();

		SDL_Delay(16);
	}

	// ------------ Cleanup -------------
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}