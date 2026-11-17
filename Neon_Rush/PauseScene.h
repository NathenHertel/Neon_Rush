#pragma once

#include "raylib.h"
#include "Config.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "MenuUI.h"

// Overlay pushed by PlayScene. (was: `if (menuOpen) { ... }`)
class PauseScene : public Scene
{
    GameContext& ctx;

    static Rectangle FullscreenButton() { return { FullscreenX, FullscreenY, 175, 30 }; }
    static Rectangle QuitButton() { return { QuitX, QuitY, 70, 30 }; }
    static Rectangle MainMenuButton() { return { FullscreenX, QuitY + 45, 175, 30 }; }

public:
    explicit PauseScene(GameContext& c) : ctx(c) {}

    bool IsOverlay() const override { return true; }

    void Update(float) override
    {
        if (IsKeyPressed(KEY_ESCAPE)) { scenes->Pop(); return; }

        if (Clicked(FullscreenButton())) ToggleFullscreenMode();
        if (Clicked(MainMenuButton())) { ctx.goToMenu(); return; }
        if (Clicked(QuitButton()))       ctx.quit = true;
    }

    void Render() override
    {
        const char* title = "Settings";
        const int fontSize = 30;
        int x = screenWidth / 2 - MeasureText(title, fontSize) / 2;
        int y = screenHeight / 2 - fontSize / 2;

        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));
        DrawText(title, x, y - 300, fontSize, WHITE);

        DrawRectangleRec(FullscreenButton(), RED);
        DrawText("FullScreen", FullscreenX + 5, FullscreenY, fontSize, WHITE);

        DrawRectangleRec(QuitButton(), RED);
        DrawText("Quit", QuitX + 5, QuitY, fontSize, WHITE);

        DrawRectangleRec(MainMenuButton(), RED);
        DrawText("Main Menu", (int)MainMenuButton().x + 5, (int)MainMenuButton().y, fontSize, WHITE);
    }
};