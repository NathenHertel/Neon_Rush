#pragma once

#include <functional>
#include <random>
#include <fstream>

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
    
    bool loadOnStart = false;            // main menu sets this, PlayScene reads it
    std::function<bool()> saveGame;      // PlayScene fills this in, PauseScene calls it

    int highScore = 0;

    void LoadHighScore()
    {
        std::ifstream file("Highscore.txt");
        int savedScore = 0;

        if (file >> savedScore && savedScore > highScore)
        {
            highScore = savedScore;
        }
    };

    void UpdateHighScore(int score)
    {
        if (score <= highScore) return;

        highScore = score;

        std::ofstream file("Highscore.txt");

        if (file)
        {
            file << highScore << std::endl;
        }
    };

    int finalScore = 0;
};