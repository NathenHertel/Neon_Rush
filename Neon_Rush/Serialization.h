#pragma once

#include "raylib.h"
#include "Agent.h"
#include "Player.h"
#include "Projectile.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using json = nlohmann::json;


inline void to_json(json& j, const Vector2& v)
{
    j = json{ {"x", v.x}, {"y", v.y} };
}

inline void from_json(const json& j, Vector2& v)
{
    v.x = j.value("x", 0.0f);
    v.y = j.value("y", 0.0f);
}

inline void to_json(json& j, const Color& c)
{
    j = json{ {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

inline void from_json(const json& j, Color& c)
{
    c.r = static_cast<unsigned char>(j.value("r", 255));
    c.g = static_cast<unsigned char>(j.value("g", 255));
    c.b = static_cast<unsigned char>(j.value("b", 255));
    c.a = static_cast<unsigned char>(j.value("a", 255));
}

// ---------------------------------------------------------------------------
// Enemies (AI_Agent)


inline void to_json(json& j, const AI_Agent& a)
{
    j = json{
        {"pos",      a._Pos},
        {"radius",   a._radius},
        {"colour",   a._colour},
        {"maxSpeed", a.maxSpeed},
        {"active",   a.active}
    };
}

inline void from_json(const json& j, AI_Agent& a)
{
    // Explicit defaults on purpose: AI_Agent() leaves 'active' uninitialised.
    a._Pos = j.value("pos", Vector2{ 0.0f, 0.0f });
    a._radius = j.value("radius", 20.0f);
    a._colour = j.value("colour", BLUE);
    a.maxSpeed = j.value("maxSpeed", 150.0f);
    a.active = j.value("active", true);
}

// ---------------------------------------------------------------------------
// Projectiles (Bullet)
// ---------------------------------------------------------------------------

inline void to_json(json& j, const Bullet& b)
{
    j = json{
        {"pos",      b._pos},
        {"vel",      b._vel},
        {"radius",   b._radius},
        {"colour",   b._colour},
        {"disabled", b._disabled}
    };
}

inline void from_json(const json& j, Bullet& b)
{
    // Explicit defaults on purpose: Bullet() leaves _pos/_vel/_radius uninitialised.
    b._pos = j.value("pos", Vector2{ 0.0f, 0.0f });
    b._vel = j.value("vel", Vector2{ 0.0f, 0.0f });
    b._radius = j.value("radius", 5.0f);
    b._colour = j.value("colour", RED);
    b._disabled = j.value("disabled", false);
}

// ---------------------------------------------------------------------------
// Player
// ---------------------------------------------------------------------------

inline void to_json(json& j, const Player& p)
{
    j = json{
        {"pos",               p._pos},
        {"radius",            p._radius},
        {"health",            p.health},
        {"max health",        p.maxHealth},
        {"level",             p.Level},
        {"amountOfShots",     p.amountOfShots},
        {"masterAmount",      p.masterAmount},
        {"levelUp",           p.levelUp},
        {"delayBetweenShots", p.delayBetweenShots},
        {"exp",               p._exp},
        {"requiredExp",       p.requiredExp}
    };
}

inline void from_json(const json& j, Player& p)
{
    // A missing key keeps whatever value the player already has.
    p._pos = j.value("pos", p._pos);
    p._radius = j.value("radius", p._radius);
    p.health = j.value("health", p.health);
    p.maxHealth = j.value("maxHealth", p.maxHealth);
    p.Level = j.value("level", p.Level);
    p.amountOfShots = j.value("amountOfShots", p.amountOfShots);
    p.masterAmount = j.value("masterAmount", p.masterAmount);
    p.levelUp = j.value("levelUp", p.levelUp);
    p.delayBetweenShots = j.value("delayBetweenShots", p.delayBetweenShots);
    p._exp = j.value("exp", p._exp);
    p.requiredExp = j.value("requiredExp", p.requiredExp);
    p.bulletsToSpawn = 0;
}

// ---------------------------------------------------------------------------
// Save / load
// ---------------------------------------------------------------------------

// Returns false if the file can't be written.
inline bool SaveGame(const std::string& path,
    const Player& player,
    const std::vector<AI_Agent>& enemies,
    const std::vector<Bullet>& bullets,
    int score)
{
    json root;
    root["version"] = 1;
    root["score"] = score;
    root["player"] = player;
    root["enemies"] = enemies;
    root["bullets"] = bullets;

    std::ofstream file(path);
    if (!file) return false;

    file << root.dump(2);   // 2 = readable formatting
    return file.good();
}

// Returns false if the file is missing or invalid.
// game state is only changed if the whole file loads successfully.
inline bool LoadGame(const std::string& path,
    Player& player,
    std::vector<AI_Agent>& enemies,
    std::vector<Bullet>& bullets,
    int& score)
{
    std::ifstream file(path);
    if (!file) return false;

    json root = json::parse(file, nullptr, false);   // false = don't throw on bad JSON
    if (root.is_discarded() || !root.is_object()) return false;

    try
    {
        Player newPlayer = player;
        if (root.contains("player")) root.at("player").get_to(newPlayer);

        auto newEnemies = root.value("enemies", std::vector<AI_Agent>{});
        auto newBullets = root.value("bullets", std::vector<Bullet>{});
        int  newScore = root.value("score", 0);

        player = newPlayer;
        enemies = std::move(newEnemies);
        bullets = std::move(newBullets);
        score = newScore;
    }
    catch (const std::exception&)
    {
        return false;
    }
    return true;
}