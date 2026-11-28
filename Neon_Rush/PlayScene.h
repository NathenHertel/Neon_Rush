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
#include "Balance.h"
#include "Hud.h"
#include "GamepadInput.h"

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

    float   waveFx = 0.6f;          // >= 0.6 means "not playing"
    Vector2 waveFxPos{ 0, 0 };
    float   bannerTimer = 0.0f;

    bool    usingPad = false;
    Vector2 padAim{ 0.0f, -1.0f };      // last right-stick direction



public:

    explicit PlayScene(GameContext& c)
        : ctx(c), spawner(0.5f, player.EnemyAmt)
    {
        ctx.finalScore = 0;
        player._pos = { screenWidth / 2, screenHeight / 2 };
        player._radius = 40.0f;

        player.EnemyAmt = Balance::MaxEnemies(0);
        spawner.SetLevel(0);
        ResetShopCosts();
    }
	
	
    // Called by SceneManager when this scene becomes active.
    void OnEnter() override
    {
        // Lets the pause menu trigger a save without knowing about PlayScene.
        ctx.saveGame = [this] { return Save(); };

        // Set by the main menu's "Load Game" button.
        if (ctx.loadOnStart)
        {
            ctx.loadOnStart = false;
            Load();
        }
    }

    // Clear the callback so the pause menu can't call into a destroyed scene.
    void OnExit() override { ctx.saveGame = nullptr; }

    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_ESCAPE) || Pad::Start()) { scenes->Push<PauseScene>(ctx); return; }
        if (IsKeyPressed(KEY_TAB) || Pad::Back()) { scenes->Push<ShopScene>(player); return; }
        if (IsKeyPressed(KEY_F5)) Save();
        if (IsKeyPressed(KEY_F9)) Load();
        if (IsKeyPressed(KEY_Q) || Pad::Bomb() && player.TryWaveClear()) WaveClear();
        if (waveFx < 0.6f) waveFx += dt;
        if (bannerTimer > 0.0f) bannerTimer -= dt;


        UpdateDeathParticles(deathParticles, dt);

        fireTimer += dt;
        dmgTimer += dt;

        spawner.Update(dt, enemies, ctx.rng);
        player.HandleMovement(dt);

        // ----- Shooting -----
        //Vector2 mouse = GetMousePosition();
        Vector2 mouse = GetAimPoint();

        if ((IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyDown(KEY_SPACE) || Pad::Fire()) &&
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
            player.levelUp = false;
            player.EnemyAmt = Balance::MaxEnemies(player.level);
            spawner.SetLevel(player.level);
            if (player.level % 5 == 0) spawner.SpawnMiniBoss();
            bannerTimer = 1.5f;
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

		// Draw HUD
        Hud::Draw(player, ctx.assets.font);

        if (waveFx < 0.6f)
        {
            float t = waveFx / 0.6f, r = t * 1500.0f;
            DrawRing(waveFxPos, fmaxf(0.0f, r - 12.0f), r, 0.0f, 360.0f, 96, Fade(SKYBLUE, 1.0f - t));
        }
        if (bannerTimer > 0.0f)
        {
            const char* msg = TextFormat("LEVEL %d", player.level);
            Vector2 ms = MeasureTextEx(ctx.assets.font, msg, 48, 2.0f);
            DrawTextEx(ctx.assets.font, msg, { (screenWidth - ms.x) / 2.0f, 110.0f }, 48, 2.0f,
                Fade(GOLD, fminf(1.0f, bannerTimer / 0.5f)));
        }
    }

private:
    bool Save()
    {
        const SaveResult r = SaveGame("save.json", player, enemies, bullets, enemyBullets, deathParticles, player.score);
        if (r) std::cout << "Game Saved" << std::endl;
        else std::cout << "Save Failed: " << r.message << std::endl;
        return r.ok;
    }

    void Load()
    {
        const SaveResult r = LoadGame("save.json", player, enemies, bullets, enemyBullets, deathParticles, player.score);

        if (r)
        {
            gameOver = false;
            fireTimer = 0.0f;
            dmgTimer = DMG_INTERVAL;
            player.EnemyAmt = Balance::MaxEnemies(player.level);
            spawner.SetLevel(player.level);
            spawner.Reset(player.EnemyAmt);
            std::cout << "Game Loaded" << std::endl;
        }
        else
        {
            std::cout << "Load Failed: " << r.message << std::endl;
        }
    }

    void WaveClear()
    {
        for (auto& e : enemies)
        {
          if (!e.active) continue;

          const bool isBoss = (e.type == EnemyType::MiniBossH || e.type == EnemyType::MiniBossSp);
          if (isBoss)
          {
              e.Health -= e.maxHealth * 0.4f;
              if (e.Health > 0.0f) continue;
          }

            SpawnDeathParticles(deathParticles, e.GetPos(), ctx.rng);
            player.points += e.pointVal / 2;
            e.active = false;
        }
         enemyBullets.clear();

         PlaySound(ctx.assets.EnemyDeath);
         spawner.Reset(player.EnemyAmt);

         waveFx = 0.0f;                 // <-- these two start the animation
         waveFxPos = player._pos;
        
    }

    // Where the ship aims: the right stick if it was used last, otherwise the mouse.
    Vector2 GetAimPoint()
    {
        if (Vector2Length(GetMouseDelta()) > 0.0f) usingPad = false;   // mouse moved: switch back

        Vector2 aim = Pad::Aim();
        if (aim.x != 0.0f || aim.y != 0.0f)
        {
            usingPad = true;
            padAim = Vector2Normalize(aim);          // remembered, so the ship keeps facing there
        }

        if (usingPad)
            return Vector2Add(player._pos, Vector2Scale(padAim, 300.0f));
        return GetMousePosition();
    }

};