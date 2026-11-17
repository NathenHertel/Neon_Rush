// ---------- DONT REMOVE ----------
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
    InitWindow(screenWidth, screenHeight, "Neon Rush");
    InitAudioDevice();
    Music backgroundMusic = LoadMusicStream("Music/BackgroundMusic.mp3");
    backgroundMusic.looping = true;
    SetMusicVolume(backgroundMusic, 0.25f);
    PlayMusicStream(backgroundMusic);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Assets assets;
    assets.Load();

    std::mt19937 rng{ std::random_device{}() };

    // Declared in this order so scenes are destroyed before the things they point at.
    GameContext  ctx{ assets, rng };
    SceneManager scenes;

    ctx.startGame = [&] { scenes.Reset<PlayScene>(ctx); };
    ctx.goToMenu = [&] { scenes.Reset<MainMenuScene>(ctx); };

    ctx.goToMenu();      // boot into the main menu
    scenes.Flush();      // apply that first request before the loop starts

    // The whole game is drawn at a fixed screenWidth x screenHeight into this texture,
    // then scaled to whatever size the window is.
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);


    while (!WindowShouldClose() && !ctx.quit)
    {
        UpdateMusicStream(backgroundMusic);

        // Keep all your existing game code here

        // ----- work out how the game image fits the window (letterboxed) -----
        const float winW = (float)GetScreenWidth();
        const float winH = (float)GetScreenHeight();
        const float scale = std::min(winW / screenWidth, winH / screenHeight);
        const float destW = screenWidth * scale;
        const float destH = screenHeight * scale;
        const float offX = (winW - destW) * 0.5f;
        const float offY = (winH - destH) * 0.5f;

        // Make GetMousePosition() return game coordinates (0..screenWidth, 0..screenHeight)
        SetMouseOffset((int)-offX, (int)-offY);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        scenes.Update(GetFrameTime());

        // ----- draw the game at its fixed size -----
        BeginTextureMode(target);
        ClearBackground({ 5, 5, 20, 255 });
        scenes.Render();
        EndTextureMode();

        // ----- draw the texture scaled to the window -----
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture,
            { 0.0f, 0.0f, (float)screenWidth, -(float)screenHeight },  // negative height: render textures are upside down
            { offX, offY, destW, destH },
            { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadMusicStream(backgroundMusic);
    UnloadRenderTexture(target);
    assets.Unload();
    CloseAudioDevice();
    CloseWindow();
    
}

int main()
{
    RunWindow();
    return 0;
}