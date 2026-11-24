#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#include "raylib.h"
#include <algorithm>
#include <random>

#include "Config.h"
#include "Assets.h"
#include "SceneManager.h"
#include "GameContext.h"
#include "MainMenuScene.h"
#include "PlayScene.h"

void RunWindow()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Neon Rush");
    InitAudioDevice();
    Music backgroundMusic = LoadMusicStream("Music/BackgroundMusic.mp3");
    backgroundMusic.looping = true;
    SetMusicVolume(backgroundMusic, 0.25);
    PlayMusicStream(backgroundMusic);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Assets assets;
    assets.Load();

    std::mt19937 rng{ std::random_device{}() };

    GameContext  ctx{ assets, rng };
    ctx.LoadHighScore();
    SceneManager scenes;

    ctx.startGame = [&] { scenes.Reset<PlayScene>(ctx); };
    ctx.goToMenu = [&] { scenes.Reset<MainMenuScene>(ctx); };

    ctx.goToMenu();
    scenes.Flush();

    while (!WindowShouldClose() && !ctx.quit)
    {
        UpdateMusicStream(backgroundMusic);

        const float winW = (float)GetScreenWidth();
        const float winH = (float)GetScreenHeight();
        const float scale = std::min(winW / screenWidth, winH / screenHeight);
        const float offX = (winW - screenWidth * scale) * 0.5f;
        const float offY = (winH - screenHeight * scale) * 0.5f;

        SetMouseOffset((int)-offX, (int)-offY);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        Camera2D camera = {};
        camera.offset = { offX, offY };
        camera.target = { 0.0f, 0.0f };
        camera.zoom = scale;

        scenes.Update(GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        DrawRectangle(0, 0, screenWidth, screenHeight, { 5, 5, 20, 255 });
        scenes.Render();
        EndMode2D();
        EndDrawing();
    }

    UnloadMusicStream(backgroundMusic);
    assets.Unload();
    CloseAudioDevice();
    CloseWindow();
}

int main()
{
    RunWindow();
    return 0;
}