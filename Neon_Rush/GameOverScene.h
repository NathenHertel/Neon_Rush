#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"

// Overlay pushed by PlayScene. (was: `if (gameOver) { ... }`)
class GameOverScene : public Scene
{
    GameContext& ctx;

public:
    explicit GameOverScene(GameContext& c) : ctx(c) {}

    bool IsOverlay() const override { return true; }

    void Update(float) override
    {
        if (IsKeyPressed(KEY_R)) ctx.startGame();
        if (IsKeyPressed(KEY_M)) ctx.goToMenu();
    }

    void Render() override
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
        DrawText("GAME OVER", screenWidth / 2 - 152, screenHeight / 2 - 40, 50, RED);
        DrawText("Press R to Restart", screenWidth / 2 - 120, screenHeight / 2 + 20, 24, WHITE);
        DrawText("Press M for Main Menu", screenWidth / 2 - 135, screenHeight / 2 + 55, 24, LIGHTGRAY);
    }
};