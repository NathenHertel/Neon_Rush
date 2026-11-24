#pragma once

#include "raylib.h"
#include "Config.h"
#include "Player.h"
#include "LevelUp.h"
#include "EnemySpawning.h"
#include "SceneManager.h"

// Overlay pushed by PlayScene. (was: `else if (player.levelUp) {}`)
class LevelUpScene : public Scene
{
    Player& player;
    EnemySpawner& spawner;
    Sound Upgrade;

public:
    LevelUpScene(Player& p, EnemySpawner& s, Sound upgrade) : player(p), spawner(s), Upgrade(upgrade) { }
    bool IsOverlay() const override { return true; }

    void Update(float) override
    {
        // DrawLevelUpMenu() handles the clicks and clears player.levelUp when an upgrade is picked.
        if (!player.levelUp)
        {
            spawner.Reset(player.EnemyAmt);   // once, instead of every frame
            scenes->Pop();
        }
    }

    void Render() override
    {
        DrawLevelUpMenu(&player, GetMousePosition(), screenWidth, screenHeight, Upgrade);
    }
};