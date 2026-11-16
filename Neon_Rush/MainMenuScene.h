#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "MenuUI.h"

// Full-screen scene (not an overlay).
class MainMenuScene : public Scene
{
    GameContext& ctx;

public:
    explicit MainMenuScene(GameContext& c) : ctx(c) {}

    void Update(float) override
    {
        if (IsKeyPressed(KEY_ENTER) || Clicked(MenuButton(0))) { ctx.startGame(); return; }
        if (Clicked(MenuButton(1)))                            ToggleFullscreenMode();
        if (Clicked(MenuButton(2)))                            ctx.quit = true;
    }

    void Render() override
    {
        const char* title = "NEON RUSH";
        const int fs = 70;
        DrawText(title, screenWidth / 2 - MeasureText(title, fs) / 2, screenHeight / 4, fs, GREEN);

        const char* hint = "Press ENTER to play";
        DrawText(hint, screenWidth / 2 - MeasureText(hint, 20) / 2, screenHeight / 4 + fs + 10, 20, LIGHTGRAY);

        DrawMenuButton(MenuButton(0), "Play");
        DrawMenuButton(MenuButton(1), "Fullscreen");
        DrawMenuButton(MenuButton(2), "Quit");
    }
};