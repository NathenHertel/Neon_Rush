#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "GamepadInput.h"
#include "MenuUI.h"

class PauseScene : public Scene
{
    GameContext& ctx;
    const char* status = nullptr;
    float statusTimer = 0.0f;
    int selected = 0;

    static Rectangle CenteredButton(int index, float w, float h)
    {
        const float gap = 15;
        const float count = 4;
        const float totalH = count * h + (count - 1) * gap;

        return {
            (screenWidth - w) / 2,
            (screenHeight - totalH) / 2 + index * (h + gap),
            w, h
        };
    }

    static Rectangle FullscreenButton() { return CenteredButton(0, 220, 40); }
    static Rectangle SaveButton() { return CenteredButton(1, 220, 40); }
    static Rectangle MainMenuButton() { return CenteredButton(2, 220, 40); }
    static Rectangle QuitButton() { return CenteredButton(3, 220, 40); }

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

    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_ESCAPE) || Pad::Start() || Pad::B()) { scenes->Pop(); return; }

        if (statusTimer > 0.0f) statusTimer -= dt;

        if (Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_DOWN)) selected = (selected + 1) % 4;
        if (Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_UP))   selected = (selected + 3) % 4;
        const bool pick = Pad::A();

        if (Clicked(FullscreenButton()) || (pick && selected == 0)) ToggleBorderlessWindowed();

        if ((Clicked(SaveButton()) || (pick && selected == 1)) && ctx.saveGame)
        {
            status = ctx.saveGame() ? "Game saved" : "Save failed";
            statusTimer = 2.0f;
        }

        if (Clicked(MainMenuButton()) || (pick && selected == 2)) { ctx.goToMenu(); return; }
        if (Clicked(QuitButton()) || (pick && selected == 3))     ctx.quit = true;
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
        DrawButton(SaveButton(), "Save Game", buttonFontSize, spacing);
        DrawButton(MainMenuButton(), "Main Menu", buttonFontSize, spacing);
        DrawButton(QuitButton(), "Quit", buttonFontSize, spacing);

        if (Pad::Available())
            DrawRectangleLinesEx(CenteredButton(selected, 220, 40), 3.0f, YELLOW);
    }
};