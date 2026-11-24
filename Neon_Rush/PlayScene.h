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

#include "Assets.h"

// Overlays this scene can push
#include "PauseScene.h"
#include "LevelUpScene.h"
#include "GameOverScene.h"
#include "ShopScene.h"

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

    bool  gameOver = false;
    float fireTimer = 0.0f;
    float dmgTimer = DMG_INTERVAL;

public:

    explicit PlayScene(GameContext& c)
        : ctx(c), spawner(0.5f, player.EnemyAmt)
    {
        ctx.finalScore = 0;
        player._pos = { screenWidth / 2, screenHeight / 2 };
        player._radius = 40.0f;
    }

    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_ESCAPE)) { scenes->Push<PauseScene>(ctx); return; }
        if (IsKeyPressed(KEY_TAB)) { scenes->Push<ShopScene>(player); return; }
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

        player.PowerUpdate(dt);

        player.UpdateAim(mouse);

        size_t bulletsBefore = bullets.size();

        player.UpdateShooting(mouse, bullets, dt, fireTimer, screenWidth, screenHeight);

        if (bullets.size() > bulletsBefore)
        {
            PlaySound(ctx.assets.LaserShot);
        }

        for (auto& b : bullets)
            b.Update(dt, screenWidth, screenHeight);

        // ----- AI + collision -----
        AI_Agent::Update(dt, player._pos, enemies, enemyBullets);

        PlayerProjectileCheck(enemyBullets, player, gameOver, dt);

        ctx.UpdateHighScore(player.score);

        EnemyProjectileCheck(bullets, enemies, deathParticles, player, player.score, ctx.rng, player.damage, ctx.assets.EnemyDeath);
        ResetCollision(enemies, bullets, enemyBullets);

        if (dmgTimer >= DMG_INTERVAL)
        {
            EnemyDamage(gameOver, enemies, player, ctx.assets.enemyVoice);
            dmgTimer = 0.0f;
        }

        // ----- State transitions: push an overlay instead of flipping a bool -----
        if (gameOver)
        {
            ctx.finalScore = player.score;
            spawner.ResetAll(player.EnemyAmt);
            scenes->Push<GameOverScene>(ctx);
        }
        else if (player.levelUp)
        {
            int next = player.level + 1;
            if (next % 5 == 0)
            {
                std::cout << "scaled" << std::endl;
                spawner.ScaleAllEnemies(enemies, spawner);
            }

            if (next % 5 == 0)
            {
                spawner.SpawnMiniBoss();
            }
            
            scenes->Push<LevelUpScene>(player, spawner, ctx.assets.font);
        }
    }

    // Runs while an overlay is on top: keeps death particles animating behind menus.
    void UpdateCovered(float dt) override
    {
        UpdateDeathParticles(deathParticles, dt);
    }

    void Render() override
    {
        player.DrawPlayer(ctx.assets.player);

        for (auto& enemy : enemies)
        {
            if (enemy.active && enemy.type == EnemyType::MiniBossSp)
                enemy.drawAura();
            if (enemy.active && enemy.type == EnemyType::MiniBossH)
                enemy.drawAura();
        }

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
                e.Draw(ctx.assets.enemy, screenWidth, screenHeight);
                break;
            case EnemyType::Ranged:
                e.Draw(ctx.assets.enemy, screenWidth, screenHeight);
                break;
            case EnemyType::MiniBossH:
                e.DrawSquare();
                break;
            case EnemyType::MiniBossSp:
                e.DrawSquare();
                break;
            }
        }

        for (auto& b : enemyBullets) b.Draw();
        for (auto& b : bullets)      b.Draw();

        DrawDeathParticles(deathParticles);

        const char* hud = TextFormat("Score: %d, Points: %d, Health: %d/%d, Level: %d",
            player.score, player.points, (int)std::round(player.health), (int)std::round(player.maxHealth), player.level);

        Vector2 size = MeasureTextEx(ctx.assets.font, hud, 20, 1.0f);
        DrawRectangle(5, 5, (int)size.x + 8, 26, BLACK);
        DrawTextEx(ctx.assets.font, hud, { 6, 6 }, 20, 1.0f, GREEN);

        const float highScoreFontSize = 24.0f;
        const float highScoreSpacing = 5.0f;
        const int padding = 40;

        const char* highScoreText =
            TextFormat("High Score: %d", ctx.highScore);

        const Vector2 textSize = MeasureTextEx(ctx.assets.font, highScoreText, highScoreFontSize, highScoreSpacing);

        const float highScoreX = screenWidth - padding - textSize.x;

        DrawRectangle(highScoreX - 5, 5, textSize.x + 10, textSize.y + 8, BLACK);

        DrawTextEx(ctx.assets.font, highScoreText, { highScoreX, 9 }, highScoreFontSize, highScoreSpacing, GOLD);
    }

private:
    void Save()
    {
        const SaveResult r = SaveGame("save.json", player, enemies, bullets, enemyBullets, deathParticles, player.score);
        if (r) std::cout << "Game Saved" << std::endl;
        else std::cout << "Save Failed: " << r.message << std::endl;
    }

    void Load()
    {
        const SaveResult r = LoadGame("save.json", player, enemies, bullets, enemyBullets, deathParticles, player.score);

        if (r)
        {
            gameOver = false;
            fireTimer = 0.0f;
            dmgTimer = DMG_INTERVAL;
            spawner.Reset(player.EnemyAmt);
            std::cout << "Game Loaded" << std::endl;
        }
        else
        {
            std::cout << "Load Failed: " << r.message << std::endl;
        }
    }

};