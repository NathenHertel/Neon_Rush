#pragma once
#include "raylib.h"
#include "raymath.h"
#include <cmath>

namespace Pad
{
    constexpr int   ID = 0;
    constexpr float DEADZONE = 0.25f;

    inline bool Available() { return IsGamepadAvailable(ID); }

    // Stick with a radial deadzone, rescaled so it ramps smoothly from 0 to 1.
    // Returns {0,0} while the stick is centred.
    inline Vector2 Stick(int axisX, int axisY)
    {
        if (!Available()) return { 0.0f, 0.0f };

        Vector2 v = { GetGamepadAxisMovement(ID, axisX), GetGamepadAxisMovement(ID, axisY) };
        float len = Vector2Length(v);
        if (len < DEADZONE) return { 0.0f, 0.0f };

        float scaled = fminf(1.0f, (len - DEADZONE) / (1.0f - DEADZONE));
        return Vector2Scale(v, scaled / len);
    }

    inline Vector2 Move() { return Stick(GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_LEFT_Y); }
    inline Vector2 Aim() { return Stick(GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_RIGHT_Y); }

    inline bool Pressed(int button) { return Available() && IsGamepadButtonPressed(ID, button); }

    // Right stick pushed, or right trigger held (some pads report triggers as axes, some as buttons)
    inline bool Fire()
    {
        if (!Available()) return false;
        Vector2 a = Aim();
        return a.x != 0.0f || a.y != 0.0f
            || GetGamepadAxisMovement(ID, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.2f
            || IsGamepadButtonDown(ID, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
    }

    inline bool Bomb() { return Pressed(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || Pressed(GAMEPAD_BUTTON_RIGHT_TRIGGER_1); }
    inline bool Start() { return Pressed(GAMEPAD_BUTTON_MIDDLE_RIGHT); }
    inline bool Back() { return Pressed(GAMEPAD_BUTTON_MIDDLE_LEFT); }
    inline bool A() { return Pressed(GAMEPAD_BUTTON_RIGHT_FACE_DOWN); }
    inline bool B() { return Pressed(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT); }
}
