#pragma once

#include <iostream>
#include <vector>

#include "raylib.h"

// ----- OUR FILES -----
#include "Agent.h"
#include "Projectile.h"
#include "Player.h"
#include "Particles.h"
#include "Config.h"
#include "EnemySpawning.h"
#include "Collision.h"
#include "Serialization.h"

#include "SceneManager.h"
#include "GameContext.h"

// Overlays this scene can push
#include "PauseScene.h"
#include "LevelUpScene.h"
#include "GameOverScene.h"

// Everything that used to be locals inside RunWindow().
class PlayScene : public Scene
{
    static constexpr float DMG_INTERVAL = 0.5f;

    GameContext& ctx;

    Player        player;
    EnemySpawner  spawner;                    // must be declared after `player`

    std::vector<AI_Agent>      enemies;
    std::vector<AI_Agent>      enemiesSquare;
    std::vector<AI_Agent>      enemiesTriangle;
    std::vector<Bullet>        bullets;
    std::vector<Bullet>        enemyBullets;
    std::vector<DeathParticle> deathParticles;

    int   score = 0;
    bool  gameOver = false;
    float fireTimer = 0.0f;
    float dmgTimer = DMG_INTERVAL;

public:
    explicit PlayScene(GameContext& c)
        : ctx(c), spawner(0.5f, player.EnemyAmt)
    {
        player._pos = { screenWidth / 2, screenHeight / 2 };
        player._radius = 40.0f;
    }

    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_ESCAPE)) { scenes->Push<PauseScene>(ctx); return; }
        if (IsKeyPressed(KEY_F5)) Save();
        if (IsKeyPressed(KEY_F9)) Load();

        UpdateDeathParticles(deathParticles, dt);

        fireTimer += dt;
        dmgTimer += dt;

        spawner.Update(dt, enemies, ctx.rng);
        player.HandleMovement(dt);

        // ----- Shooting -----
        Vector2 mouse = GetMousePosition();

        if ((IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyDown(KEY_SPACE)) &&
            fireTimer >= player.fireInterval)
        {
            player.StartBurst(fireTimer);
        }

        player.UpdateAim(mouse);

        size_t bulletsBefore = bullets.size();

        player.UpdateShooting(mouse, bullets, dt, fireTimer, screenWidth, screenHeight);

        // Play laser sound when a bullet is fired
        if (bullets.size() > bulletsBefore)
        {
            PlaySound(ctx.assets.laserShot);
        }


        for (auto& b : bullets)
            b.Update(dt, screenWidth, screenHeight);

        // ----- AI + collision -----
        AI_Agent::Update(dt, player._pos, enemies, enemyBullets);

        PlayerProjectileCheck(enemyBullets, player, gameOver, dt);

        ctx.UpdateHighScore(score);

        EnemyProjectileCheck(bullets,enemies,deathParticles,player,score,ctx.rng,player.damage,ctx.assets.EnemyDeath);

        ResetCollision(enemies, bullets, enemyBullets);

        if (dmgTimer >= DMG_INTERVAL)
        {
            EnemyDamage(gameOver, enemies, player, ctx.assets.enemyVoice);
            dmgTimer = 0.0f;
        }

        // ----- State transitions: push an overlay instead of flipping a bool -----
        if (gameOver)
            scenes->Push<GameOverScene>(ctx);
        else if (player.levelUp)
            scenes->Push<LevelUpScene>(player, spawner,ctx.assets.Upgrade);
    }

    // Runs while an overlay is on top: keeps death particles animating behind menus.
    void UpdateCovered(float dt) override
    {
        UpdateDeathParticles(deathParticles, dt);
    }

    void Render() override
    {
        player.DrawPlayer(ctx.assets.player);

        for (auto& e : enemies)
        {
            if (!e.active) continue;

            switch (e.type)
            {
            case EnemyType::Square:
                e.DrawSquare();
                break;
            case EnemyType::Triangle:
                e.DrawTriangle();   // whatever you named the triangle draw function
                break;
            case EnemyType::Melee:
            case EnemyType::Ranged:
                e.Draw(ctx.assets.enemy, screenWidth, screenHeight);
                break;
            }
        }

        for (auto& b : enemyBullets) b.Draw();
        for (auto& b : bullets)      b.Draw();

        DrawDeathParticles(deathParticles);

        const char* hud = TextFormat("Score: %d, Health: %d/%d, Level: %d, Enemy Amount: %d",
            score, player.health, player.maxHealth, player.Level, player.EnemyAmt);
        DrawRectangle(5, 5, MeasureText(hud, 20) + 8, 24, BLACK);
        DrawText(hud, 6, 6, 20, GREEN);
        const int highScoreFontSize = 24;
        const int padding = 10;

        const char* highScoreText =
            TextFormat("High Score: %d", ctx.highScore);

        const int highScoreWidth =
            MeasureText(highScoreText, highScoreFontSize);

        const int highScoreX =
            screenWidth - padding - highScoreWidth;

        DrawRectangle(highScoreX - 5, 5,
            highScoreWidth + 10,
            highScoreFontSize + 8, BLACK);

        DrawText(highScoreText, highScoreX, 9,
            highScoreFontSize, GOLD);
    }

private:
    void Save()
    {
        if (SaveGame("save.json", player, enemies, bullets, score))
            std::cout << "Game Saved" << std::endl;
        else
            std::cout << "Save FAILED: could not write save.json" << std::endl;
    }

    void Load()
    {
        if (LoadGame("save.json", player, enemies, bullets, score))
        {
            gameOver = false;
            fireTimer = 0.0f;
            std::cout << "Game Loaded" << std::endl;
        }
        else
        {
            std::cout << "Load FAILED: save.json is missing or invalid" << std::endl;
        }
    }
};