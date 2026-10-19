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
    InitAudioDevice();

    SetExitKey(KEY_NULL);

    Player player;
    Sound EnemyVoice = LoadSound("spongebob-hi-how-are-ya-.mp3");

    player._pos = { screenWidth / 2, screenHeight / 2 };
    player._radius = 40.0f;

    // ----------- RANDOM GENERATION FOR ENEMY PLACEMENT ----------
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> disX(20.0f, screenWidth - 20.0f);
    std::uniform_real_distribution<float> disY(20.0f, screenHeight - 20.0f);

    // ---------- VECTORS FOR MULTIPLE OF 1 CLASS ----------
    std::vector<AI_Agent> Enemies;
    std::vector<Bullet> Bullets;

    std::vector<Sound> EnemySounds;

    // ---------- TIMERS ----------
    float spawnTimer = 0.0f;
    const float spawnInterval = 0.5f;

    float fireTimer = 0.0f;
    const float fireInterval = 1.0f;

    float dmgTimer = 0.5f;
    const float dmgInterval = 0.5f;

    float angle = 0.0f;

    bool gameOver = false;
    bool menuOpen = false;

    SetTargetFPS(60);

    // ---------- GAME LOOP -----------
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            menuOpen = !menuOpen;
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

        if (gameOver)
        {
            if (IsKeyPressed(KEY_R))
            {
                gameOver = false;
                score = 0;
                Enemies.clear();
                Bullets.clear();
                player._pos = { screenWidth / 2, screenHeight / 2 };
                player.health = player.baseHealth;
                player.levelUp = false;
                player._exp = 0.0f;
                player.requiredExp = 100.0f;
                spawnTimer = 0.0f;
                fireTimer = 0.0f;
                dmgTimer = dmgInterval;
            }
        }
        else if (player.levelUp)
        {
            if (IsKeyPressed(KEY_M))
            {
                player.health = player.baseHealth;
                player.levelUp = false;
            }
        }
        else if (menuOpen)
        {

        }
        else
        {
            // ----- UPDATE TIMERS -----
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
                    player.StartBurst(fireTimer);
                }
            }
            else if (IsKeyDown(KEY_SPACE))
            {
                if (fireTimer >= fireInterval)
                {
                    player.StartBurst(fireTimer);
                }
            }

            player.UpdateShooting(mousePos, pivot, Bullets, v1, deltaTime, fireTimer,
                screenWidth, screenHeight);

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
                AI_Agent.MoveToPlayer(player._pos, 350, deltaTime);
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

                        std::uniform_real_distribution<float> distrib(Enemies[j].minExp, Enemies[j].maxExp);
                        float exp = distrib(gen);

                        player.LevelUp(exp);

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
                            PlaySound(EnemyVoice);
                            gameOver = true;

                            break;
                        }
                    }
                }

                dmgTimer = 0.0f;
            }

#pragma endregion
        }

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
        DrawText(TextFormat("Score: %d, Health: %d/%d", score, player.health, player.baseHealth), 6, 6, 20, GREEN);

        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2 - 40, 50, RED);
            DrawText("Press R to Restart", screenWidth / 2 - 110, screenHeight / 2 + 20, 24, WHITE);
        }
        if (player.levelUp)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText("Level UP - press M", screenWidth / 2 - 130, screenHeight / 2, 30, WHITE);
        }
        if (menuOpen)
        {
            const char* text = "Settings";
            int fontSize = 30;

            int textWidth = MeasureText(text, fontSize);

            int x = screenWidth / 2 - textWidth / 2;
            int y = screenHeight / 2 - fontSize / 2;

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText(text, x, y, fontSize, WHITE);
        }

        EndDrawing();
    }

    UnloadSound(EnemyVoice);
    CloseAudioDevice();
    CloseWindow();

    return;
};