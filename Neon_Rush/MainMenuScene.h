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
        const Font& f = ctx.assets.font;

        const char* title = "NEON RUSH";
        const float fs = 70.0f;
        Vector2 tm = MeasureTextEx(f, title, fs, 1.0f);
        DrawTextEx(f, title, { (screenWidth - tm.x) / 2.0f, screenHeight / 4.0f }, fs, 1.0f, GREEN);

        const char* hint = "Press ENTER to play";
        Vector2 hm = MeasureTextEx(f, hint, 20, 1.0f);
        DrawTextEx(f, hint, { (screenWidth - hm.x) / 2.0f, screenHeight / 4.0f + fs + 10 }, 20, 1.0f, LIGHTGRAY);

        DrawMenuButton(MenuButton(0), "Play", f);
        DrawMenuButton(MenuButton(1), "Fullscreen", f);
        DrawMenuButton(MenuButton(2), "Quit", f);
    }
};