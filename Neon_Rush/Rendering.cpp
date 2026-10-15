// ---------- DONT REMOVE ----------
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#include "Rendering.h"
#include "raylib.h"
#include "raymath.h"
#include "Agent.h"
#include "Projectile.h"
#include "Player.h"

#include <random>
#include <vector>

// ---------- CONSTANTS ----------
const int screenWidth = 1280;
const int screenHeight = 720;

float speed = 10.0f;
float size = 45.0f;

int score = 0;

void RunWindow()
{
    InitWindow(screenWidth, screenHeight, "Neon Rush");

    Player player;

    player._pos = { screenWidth / 2, screenHeight / 2 };
    player._radius = 30.0f;

    // ----------- RANDOM GENERATION FOR ENEMY PLACEMENT ----------
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> disX(20.0f, screenWidth - 20.0f);
    std::uniform_real_distribution<float> disY(20.0f, screenHeight - 20.0f);

    // ---------- VECTORS FOR MULTIPLE OF 1 CLASS ----------
    std::vector<AI_Agent> Enemies;
    std::vector<Bullet> Bullets;

    // ---------- TIMERS ----------
    float spawnTimer = 0.0f;
    const float spawnInterval = 1.0f;

    float fireTimer = 0.0f;
    const float fireInterval = 1.0f;

    float dmgTimer = 0.0f;
    const float dmgInterval = 2.0f;

    float angle = 0.0f;

    SetTargetFPS(60);

    // ---------- GAME LOOP -----------
    while (!WindowShouldClose())
    {
        // ----- UPDATE TIMERS -----
        float deltaTime = GetFrameTime();
        spawnTimer += deltaTime;
        fireTimer += deltaTime;
        dmgTimer += deltaTime;

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

#pragma region Triangle
        // ---------- CREATE / ROTATE TRIANGLE ----------
        Vector2 o1 = { 0.0f, -50.0f };
        Vector2 o2 = { -25.0f, 20.0f };
        Vector2 o3 = { 25.0f, 20.0f };

        Vector2 c = { (o1.x + o2.x + o3.x) / 3.0f,
                      (o1.y + o2.y + o3.y) / 3.0f };

        Vector2 pivot = { player._pos.x + c.x, player._pos.y + c.y };
        Vector2 mousePos = GetMousePosition();

        float rad = atan2f(mousePos.y - pivot.y, mousePos.x - pivot.x) + PI / 2.0f;

        auto rot = [&](Vector2 o)
            {
                float dx = o.x - c.x;
                float dy = o.y - c.y;

                return Vector2
                {
                    pivot.x + dx * cosf(rad) - dy * sinf(rad),
                    pivot.y + dx * sinf(rad) + dy * cosf(rad)
                };
            };

        Vector2 v1 = rot(o1);
        Vector2 v2 = rot(o2);
        Vector2 v3 = rot(o3);

#pragma endregion

#pragma region Movement
        // ---------- MOVEMENT ----------

        // ----- RIGHT -----
        if (IsKeyDown(KEY_RIGHT)) player._pos.x += speed;
        else if (IsKeyDown(KEY_D)) player._pos.x += speed;

        // ----- LEFT -----
        if (IsKeyDown(KEY_LEFT)) player._pos.x -= speed;
        else if (IsKeyDown(KEY_A)) player._pos.x -= speed;

        // ----- UP -----
        if (IsKeyDown(KEY_UP)) player._pos.y -= speed;
        else if (IsKeyDown(KEY_W)) player._pos.y -= speed;

        // ----- DOWN -----
        if (IsKeyDown(KEY_DOWN)) player._pos.y += speed;
        else if (IsKeyDown(KEY_S)) player._pos.y += speed;

#pragma endregion

#pragma region Shooting
        // ----- SHOOT -----
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // ----- Checks if it can fire yet -----
            if (fireTimer >= fireInterval)
            {
                Bullet newBullet;
                newBullet._pos.x = v1.x;
                newBullet._pos.y = v1.y;
                newBullet._radius = 5.0f;

                float dx = mousePos.x - pivot.x;
                float dy = mousePos.y - pivot.y;

                float length = sqrtf(dx * dx + dy * dy);
                float dirX = dx / length;
                float dirY = dy / length;

                Vector2 vel = { (dirX * 500.0f), (dirY * 500.0f) };

                newBullet._vel = vel;

                newBullet.Update(deltaTime, screenWidth, screenHeight);

                Bullets.push_back(newBullet);

                fireTimer = 0.0f;
            }
        }
        else if (IsKeyDown(KEY_SPACE))
        {
            if (fireTimer >= fireInterval)
            {
                Bullet newBullet;
                newBullet._pos.x = v1.x;
                newBullet._pos.y = v1.y;
                newBullet._radius = 5.0f;

                float dx = mousePos.x - pivot.x;
                float dy = mousePos.y - pivot.y;

                float length = sqrtf(dx * dx + dy * dy);
                float dirX = dx / length;
                float dirY = dy / length;

                Vector2 vel = { (dirX * 500.0f), (dirY * 500.0f) };

                newBullet._vel = vel;

                newBullet.Update(deltaTime, screenWidth, screenHeight);

                Bullets.push_back(newBullet);

                fireTimer = 0.0f;
            }
        }

        for (auto& Bullet : Bullets)
        {
            Bullet.Update(deltaTime, screenWidth, screenHeight);
        }

        player._pos.x = Clamp(player._pos.x, player._radius, screenWidth - player._radius);
        player._pos.y = Clamp(player._pos.y, player._radius, screenHeight - player._radius);

#pragma endregion

#pragma region AI Movement
        // ---------- AI MOVEMENT ----------

        for (auto& AI_Agent : Enemies)
        {
            if (!AI_Agent.active) continue;
            AI_Agent.MoveToPlayer(player._pos, 100, deltaTime);
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

#pragma endregion

#pragma region Bullets
        // ---------- BULLET CHECK IF HIT ----------

        for (int i = 0; i < Bullets.size(); i++)
        {
            if (Bullets[i]._disabled) continue;

            for (int j = 0; j < Enemies.size(); j++)
            {
                if (!Enemies[j].active) continue;

                if (Enemies[j].CheckOverlapWithOtherAgent(Enemies[j]._Pos, Bullets[i]._pos,
                                                          Enemies[j]._radius, Bullets[i]._radius))
                {
                    Enemies[j].active = false;
                    Bullets[i]._disabled = true;

                    ++score;
                    break;
                }
            }
        }

        // ---------- BULLET HIT CLEANUP ----------

        Enemies.erase
        (
            std::remove_if(Enemies.begin(), Enemies.end(), [](const AI_Agent& e) { return !e.active; }),
            Enemies.end()
        );

        Bullets.erase
        (
            std::remove_if(Bullets.begin(), Bullets.end(), [](const Bullet& b) { return b._disabled; }),
            Bullets.end()
        );

#pragma endregion

#pragma region Enemy Damage
        // ---------- CHECK IF ENEMY HIT PLAYER ----------

        if (dmgTimer >= dmgInterval)
        {
            for (size_t i = 0; i < Enemies.size(); ++i)
            {
                AI_Agent& a = Enemies[i];

                Vector2 AIpos1 = a.GetPos();

                if (a.CheckOverlapWithOtherAgent(AIpos1, player._pos, a._radius, player._radius))
                {
                    player.setHealth(3);

                    if (player.health <= 0)
                    {
                        CloseWindow();

                        return;
                    }
                }
            }

            dmgTimer = 0.0f;
        }

#pragma endregion

        // ---------- RENDER ----------


        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTriangle(v1, v2, v3, VIOLET);

        for (auto& AI_Agent : Enemies)
        {
            if (!AI_Agent.active) continue;
            AI_Agent.Draw();
        }

        for (auto& Bullet : Bullets)
        {
            Bullet.Draw();
        }

        DrawRectangle(5, 5, 200, 20, BLACK);
        DrawText(TextFormat("Score: %d, Health: %d", score, player.health), 6, 6, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return;
};