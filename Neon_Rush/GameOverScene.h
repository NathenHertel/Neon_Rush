#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "GamepadInput.h"
#include "PlayScene.h"

// Overlay pushed by PlayScene. (was: `if (gameOver) { ... }`)
class GameOverScene : public Scene
{
    GameContext& ctx;

public:
    explicit GameOverScene(GameContext& c) : ctx(c) {}

    bool IsOverlay() const override { return true; }

    void Update(float) override
    {
        if (IsKeyPressed(KEY_R) || Pad::A()) ctx.startGame();
        if (IsKeyPressed(KEY_M) || Pad::B()) ctx.goToMenu();
    }

    void Render() override
    {
        const Font& f = ctx.assets.font;
        const char* ScoreText = TextFormat("Final Score: %d", ctx.finalScore);

        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

        DrawCentered(f, "GAME OVER", screenHeight / 2 - 100, 50, RED);
        DrawCentered(f, ScoreText, screenHeight / 2 - 30, 30, WHITE);
        DrawCentered(f, "Press R to Restart", screenHeight / 2 + 30, 24, WHITE);
        DrawCentered(f, "Press M for Main Menu", screenHeight / 2 + 65, 24, LIGHTGRAY);
    }

private:
    static void DrawCentered(const Font& f, const char* text, int y, float size, Color c)
    {
        Vector2 m = MeasureTextEx(f, text, size, 1.0f);
        DrawTextEx(f, text, { (screenWidth - m.x) / 2.0f, (float)y }, size, 1.0f, c);
    }
};