#pragma once

#include <functional>
#include <random>

#include "Assets.h"

// Things every scene needs. Created once in RunWindow().
// startGame / goToMenu are filled in by RunWindow(), so scenes never
// need to know about each other's types (no circular includes).
struct GameContext
{
    Assets& assets;
    std::mt19937& rng;
    bool          quit = false;          // set to true to leave the main loop
    std::function<void()> startGame;     // fresh PlayScene
    std::function<void()> goToMenu;      // back to MainMenuScene
};