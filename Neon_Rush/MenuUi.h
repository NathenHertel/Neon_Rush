#pragma once

#include "raylib.h"
#include "Config.h"

// Small UI helpers shared by the menu-style scenes.

inline void ToggleFullscreenMode()
{
    if (!IsWindowFullscreen())              // NOTE: needs (), the original was missing them
    {
        int m = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(m), GetMonitorHeight(m));
        ToggleFullscreen();
    }
    else
    {
        ToggleFullscreen();
        SetWindowSize(windowWidth, windowHeight);
    }
}

// i-th button in a centred vertical stack
inline Rectangle MenuButton(int i)
{
    const float w = 220.0f, h = 45.0f, gap = 15.0f;
    return { screenWidth / 2.0f - w / 2.0f, screenHeight / 2.0f + i * (h + gap), w, h };
}

inline void DrawMenuButton(Rectangle r, const char* label, Font font)
{

    bool hover = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec(r, hover ? ORANGE : RED);

    const int fs = 24;
    Vector2 ts = MeasureTextEx(font, label, fs, 1.0f);
    float x = r.x + (r.width - ts.x) / 2.0f;
    float y = r.y + (r.height - ts.y) / 2.0f;
    DrawTextEx(font, label, { x, y }, fs, 1.0f, WHITE);
}

inline bool Clicked(Rectangle r)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r);
}