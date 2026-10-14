// ---------- DONT REMOVE ----------
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#include "Rendering.h"
#include "raylib.h"
#include "raymath.h"
#include "Agent.h"


// ---------- CONSTANTS ----------
const int screenWidth = 1280;
const int screenHeight = 720;

float speed = 10.0f;
float size = 45.0f;
float radius = 30.0f;

AI_Agent* agent;

void RunWindow()
{
	InitWindow(screenWidth, screenHeight, "Neon Rush");

	Vector2 ballPos = { screenWidth / 2, screenHeight / 2 };

	agent = new AI_Agent({ 150, 150 }, 20.0f, BLUE);

	SetTargetFPS(60);

	while (!WindowShouldClose())
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

		EndDrawing();
	}

	CloseWindow();

	return;
}