#pragma once
#include <algorithm>
#include <cmath>

// Every number you are likely to tweak while balancing lives here.
namespace Balance
{
    // ----- Player -----
	constexpr float HP_PER_LEVEL = 1.0f;   // extra max health per level
	constexpr float HEAL_PER_LEVEL = 2.0f;  //health restored on level up  
	constexpr float EXP_GROWTH = 2.0f;   //exp required for next level 
    constexpr float MIN_FIRE_INTERVAL = 0.12f; // minimun fire rate

    // ----- Abilities -----
    constexpr float WAVE_CLEAR_COOLDOWN = 45.0f;  // seconds
    constexpr float SHIELD_COOLDOWN = 30.0f;  // seconds, starts when the shield breaks
    constexpr float SHIELD_BASE = 4.0f;   // shield HP
    constexpr float SHIELD_PER_UPGRADE = 2.0f;   // extra shield HP per shop purchase

    // ----- Difficulty curve (all driven by player.level) -----
    // Each one is "1 + small linear term + smaller quadratic term", so levels 0-5
    // barely change anything and the curve bends upward after that.
    inline float EnemyHealthMul(int lvl)
    {
        const float t = (float)lvl;
        return 1.0f + 0.03f * t + 0.006f * t * t;        // L5 1.3x, L10 1.9x, L20 4.0x
    }

    inline float EnemyDamageMul(int lvl)
    {
        const float t = (float)lvl;
        return 1.0f + 0.02f * t + 0.003f * t * t;        // L5 1.2x, L10 1.5x, L20 2.6x
    }

    inline float ProjectileSpeedMul(int lvl)
    {
        return std::min(2.0f, 1.0f + 0.02f * (float)lvl);  // L10 1.2x, caps at 2x (L50)
    }

    inline float ShootIntervalMul(int lvl)
    {
        return std::max(0.5f, 1.0f - 0.015f * (float)lvl); // ranged enemies fire faster, capped at 2x
    }

    inline int MaxEnemies(int lvl)
    {
        return std::min(70, 4 + 2 * lvl + (lvl * lvl) / 15);   // L5 15, L10 30, L15 49, L20 70
    }

    inline float SpawnInterval(int lvl)
    {
        return std::max(0.12f, 1.2f * powf(0.9f, (float)lvl)); // L5 0.71s, L10 0.42s, L15 0.25s, L20 0.15s
    }
}
