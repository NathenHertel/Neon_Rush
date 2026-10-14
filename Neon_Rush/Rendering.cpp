// ---------- DONT REMOVE ----------
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#include "Rendering.h"
#include "raylib.h"
#include "raymath.h"
#include "Agent.h"

#include <random>
#include <vector>

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

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> disX(20.0f, screenWidth - 20.0f);
    std::uniform_real_distribution<float> disY(20.0f, screenHeight - 20.0f);

    std::vector<AI_Agent> Enemies;

    float spawnTimer = 0.0f;
    const float spawnInterval = 1.0f;

    SetTargetFPS(60);

    // ---------- Render -----------
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        spawnTimer += deltaTime;

        // ---------- SPAWN ENEMY ----------

        if (spawnTimer >= spawnInterval && Enemies.size() <= 5 - 1)
        {
            AI_Agent newAgent;

            newAgent.SetPos({ disX(gen), disY(gen) });
            newAgent.maxSpeed = 150.0f;
            newAgent._radius = 20.0f;
            newAgent.active = true;

            Enemies.push_back(newAgent);

            spawnTimer = 0.0f;
        }

        // ---------- CREATE TRIANGLE ----------

        Vector2 v1 = { ballPos.x, ballPos.y - 50.0f };
        Vector2 v2 = { ballPos.x - 25.0f, ballPos.y + 20.0f };
        Vector2 v3 = { ballPos.x + 25.0f, ballPos.y + 20.0f };

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

        for (auto& AI_Agent : Enemies)
        {
            if (!AI_Agent.active) continue;
            AI_Agent.MoveToPlayer(ballPos, 100, deltaTime);
        }

        for (size_t i = 0; i < Enemies.size(); ++i)
        {
            for (size_t j = i + 1; j < Enemies.size(); ++j)
            {
                AI_Agent& a = Enemies[i];
                AI_Agent& b = Enemies[j];

                Vector2 AIpos1 = a.GetPos();
                Vector2 AIpos2 = b.GetPos();

                if (a.CheckOverlapWithOtherAgent(AIpos1, AIpos2, a._radius, b._radius))
                {
                    a.CollisionWithOtherAgent(AIpos1, AIpos2, a._radius, b._radius);

                    a.SetPos(AIpos1);
                    b.SetPos(AIpos2);
                }
            }
        }

        // ---------- RENDER ----------

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTriangle(v1, v2, v3, VIOLET);

        for (auto& AI_Agent : Enemies)
        {
            AI_Agent.Draw();
        }

        EndDrawing();
    }

    CloseWindow();

    return;
}