#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "MenuUI.h"

class PauseScene : public Scene
{
    GameContext& ctx;

    static Rectangle CenteredButton(int index, float w, float h)
    {
        const float gap = 15;
        const float count = 3;
        const float totalH = count * h + (count - 1) * gap;

        return {
            (screenWidth - w) / 2,
            (screenHeight - totalH) / 2 + index * (h + gap),
            w, h
        };
    }

    static Rectangle FullscreenButton() { return CenteredButton(0, 220, 40); }
    static Rectangle MainMenuButton() { return CenteredButton(1, 220, 40); }
    static Rectangle QuitButton() { return CenteredButton(2, 220, 40); }

    static void DrawTextCentered(const Font& font, const char* text, Rectangle rect,
        float fontSize, float spacing, Color color)
    {
        Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
        Vector2 pos = {
            rect.x + (rect.width - size.x) / 2,
            rect.y + (rect.height - size.y) / 2
        };
        DrawTextEx(font, text, pos, fontSize, spacing, color);
    }

    void DrawButton(Rectangle rect, const char* label, float fontSize, float spacing)
    {
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);

        DrawRectangleRec(rect, RED);
        if (hovered) DrawRectangleRec(rect, Fade(WHITE, 0.25f));

        DrawTextCentered(ctx.assets.font, label, rect, fontSize, spacing, WHITE);
    }

public:
    explicit PauseScene(GameContext& c) : ctx(c) {}

    bool IsOverlay() const override { return true; }

    void Update(float) override
    {
        if (IsKeyPressed(KEY_ESCAPE)) { scenes->Pop(); return; }

        if (Clicked(FullscreenButton())) ToggleBorderlessWindowed();
        if (Clicked(MainMenuButton())) { ctx.goToMenu(); return; }
        if (Clicked(QuitButton()))       ctx.quit = true;
    }

    void Render() override
    {
        const char* title = "Settings";
        const float titleSize = 30;
        const float buttonFontSize = 20;
        const float spacing = 5;

        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

        Vector2 titleDims = MeasureTextEx(ctx.assets.font, title, titleSize, spacing);
        float titleX = (screenWidth - titleDims.x) / 2;
        float titleY = FullscreenButton().y - titleDims.y - 30;
        DrawTextEx(ctx.assets.font, title, { titleX, titleY }, titleSize, spacing, WHITE);

        DrawButton(FullscreenButton(), "FullScreen", buttonFontSize, spacing);
        DrawButton(MainMenuButton(), "Main Menu", buttonFontSize, spacing);
        DrawButton(QuitButton(), "Quit", buttonFontSize, spacing);
    }
};