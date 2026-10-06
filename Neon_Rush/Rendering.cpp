// ---------- DONT REMOVE ----------
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#include "Rendering.h"
#include "raylib.h"
#include "raymath.h"


// ---------- CONSTANTS ----------
const int screenWidth = 1280;
const int screenHeight = 720;

float speed = 10.0f;
float size = 45.0f;
float radius = 30.0f;

void RunWindow()
{
	InitWindow(screenWidth, screenHeight, "Neon Rush");
	
	Vector2 ballPos = { screenWidth / 2, screenHeight / 2 };

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		// ---------- UPDATE ----------

		if (IsKeyDown(KEY_RIGHT)) ballPos.x += speed;
		if (IsKeyDown(KEY_LEFT)) ballPos.x -= speed;
		if (IsKeyDown(KEY_UP)) ballPos.y -= speed;
		if (IsKeyDown(KEY_DOWN)) ballPos.y += speed;

		ballPos.x = Clamp(ballPos.x, radius, screenWidth - radius);
		ballPos.y = Clamp(ballPos.y, radius, screenHeight - radius);

		// ---------- RENDER ----------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawCircleV(ballPos, radius, MAROON);

		EndDrawing();
	}

	CloseWindow();

	return;
}