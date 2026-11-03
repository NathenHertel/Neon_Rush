// ---------- DONT REMOVE ----------
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

// ----- OUR FILES -----
#include "Rendering.h"
#include "Agent.h"
#include "Projectile.h"
#include "Player.h"
#include "LevelUp.h"
#include "Particles.h"
#include "Config.h"
#include "Assets.h"
#include "EnemySpawning.h"
#include "Collision.h"
#include "Serialization.h"

#include "raylib.h"
#include "raymath.h"
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>

// ---------- CONSTANTS ----------
int monitor = GetCurrentMonitor();
int monitorWidth = GetMonitorWidth(monitor);
int monitorHeight = GetMonitorHeight(monitor);

int score = 0;

std::vector<DeathParticle> particles;
std::mt19937 rng{ std::random_device{}() };

void RunWindow()
{
    InitWindow(screenWidth, screenHeight, "Neon Rush");
    InitAudioDevice();

    Assets assets; 
    assets.Load();

    SetExitKey(KEY_NULL);

    Player player;

    player._pos = { screenWidth / 2, screenHeight / 2 };
    player._radius = 40.0f;

    EnemySpawner spawner(0.5f, player.EnemyAmt);

    // ----------- RANDOM GENERATION FOR ENEMY PLACEMENT ----------
    std::random_device rd;
    std::mt19937 gen(rd());

    // ---------- VECTORS FOR MULTIPLE OF 1 CLASS ----------
    std::vector<AI_Agent> Enemies;
    std::vector<Bullet> Bullets;
    std::vector<Bullet> EnemyBullets;
    std::vector<DeathParticle> deathParticles;

    std::vector<Sound> EnemySounds;

    // ---------- TIMERS ----------
    float spawnTimer = 0.0f;
    const float spawnInterval = 0.5f;

    float fireTimer = 0.0f;
    const float fireInterval = player.fireInterval;

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
        Vector2 mousePos = GetMousePosition();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            menuOpen = !menuOpen;
        }

        if (IsKeyPressed(KEY_F5))
        {
            if (SaveGame("save.json", player, Enemies, Bullets, score))
            {
                std::cout << "Game Saved" << std::endl;
            }
            else
            {
                std::cout << "Save FAILED: could not write save.json" << std::endl;
            }
        }

        if (IsKeyPressed(KEY_F9))
        {
            if (LoadGame("save.json", player, Enemies, Bullets, score))
            {
                gameOver = false;
                fireTimer = 0.0f;
                spawnTimer = 0.0f;

                std::cout << "Game loaded" << std::endl;
            }
            else
            {
                std::cout << "Load FAILED: save.json is missing or invalid" << std::endl;
            }
        }

        // ---------- GAME STATE LOGIC ---------- 
        // will be replaced with scenemanager
        if (gameOver)
        {
            if (IsKeyPressed(KEY_R))
            {
                gameOver = false;
                score = 0;
                Enemies.clear();
                Bullets.clear();
                EnemyBullets.clear();
                deathParticles.clear();
                fireTimer = 0.0f;
                dmgTimer = dmgInterval;

                player.Reset();
                spawner.Reset(player.EnemyAmt);
            }
        }
        else if (player.levelUp)
        {
            
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
            
            spawner.Update(deltaTime, Enemies, gen);

            player.HandleMovement(deltaTime);

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

            player.UpdateAim(mousePos);

            player.UpdateShooting(mousePos, Bullets, deltaTime, fireTimer,
                screenWidth, screenHeight);

            for (auto& Bullet : Bullets)
            {
                Bullet.Update(deltaTime, screenWidth, screenHeight);
            }

#pragma endregion

            // ---------- AI MOVEMENT ----------

            AI_Agent::Update(deltaTime, player._pos, Enemies, EnemyBullets);


            // ---------- COLLISION ----------

            PlayerProjectileCheck(EnemyBullets, player, gameOver, deltaTime);

            EnemyProjectileCheck(Bullets, Enemies, deathParticles, player, score, gen, player.damage);

            // ---------- COLLISION CLEANUP ----------

            ResetCollision(Enemies, Bullets, EnemyBullets);

            if (dmgTimer >= dmgInterval)
            {
                EnemyDamage(gameOver, Enemies, player, assets.enemyVoice);

                dmgTimer = 0.0f;
            }
        }

        UpdateDeathParticles(deathParticles, deltaTime);

        // ---------- RENDER ----------

        BeginDrawing();

        //ClearBackground(RAYWHITE);
        ClearBackground({ 5, 5, 20, 255 });

        player.DrawPlayer(assets.player);

        // ----- Draw Enemies -----
        for (auto& AI_Agent : Enemies)
        {
            if (!AI_Agent.active) continue;
            AI_Agent.Draw(assets.enemy, screenWidth, screenHeight);
        }

        // ----- Draw Enemy Bullets -----
        for (auto& b : EnemyBullets)
            b.Draw();

        for (auto& Bullet : Bullets)
        {
            Bullet.Draw();
        }

        DrawDeathParticles(deathParticles);

        DrawRectangle(5, 5, 200, 20, BLACK);
        DrawText(TextFormat("Score: %d, Health: %d/%d, Level: %d, Enemy Amount: %d", score, player.health, player.maxHealth, player.Level, player.EnemyAmt), 6, 6, 20, GREEN);

        // ---------- RENDER DIFFRENT STUFF DEPENDING ON GAME STATE ----------
        if (gameOver)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText("GAME OVER", screenWidth / 2 - 152, screenHeight / 2 - 40, 50, RED);
            DrawText("Press R to Restart", screenWidth / 2 - 120, screenHeight / 2 + 20, 24, WHITE);
        }
        if (player.levelUp)
        {
            DrawLevelUpMenu(&player, mousePos, screenWidth, screenHeight);
            spawner.Reset(player.EnemyAmt);
        }
        if (menuOpen)
        {
            const char* text = "Settings";
            int fontSize = 30;

            int textWidth = MeasureText(text, fontSize);

            int x = screenWidth / 2 - textWidth / 2;
            int y = screenHeight / 2 - fontSize / 2;

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
            DrawText(text, x, y - 300, fontSize, WHITE);

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

            Rectangle FullscreenToggle = { FullscreenX, FullscreenY, 175, 30 };
            Rectangle Quit = { QuitX, QuitY, 70, 30 };

            // ----- SETTINGS BUTTONS ----- 

            DrawRectangleRec(FullscreenToggle, RED);
            DrawText("FullScreen", FullscreenX + 5, FullscreenY, fontSize, WHITE);

            DrawRectangleRec(Quit, RED);
            DrawText("Quit", QuitX + 5, QuitY, fontSize, WHITE);

            if (CheckCollisionPointRec(mousePos, FullscreenToggle) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (!IsWindowFullscreen)
                {
                    SetWindowSize(monitorWidth, monitorHeight);
                    ToggleFullscreen();
                }
                else
                {
                    ToggleFullscreen();
                    SetWindowSize(screenWidth, screenHeight);
                }
            }

            if (CheckCollisionPointRec(mousePos, Quit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                return;
            }
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();

    return;
};