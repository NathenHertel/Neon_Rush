#pragma once

#include "raylib.h"
#include "Agent.h"        // AI_Agent, EnemyType
#include "Player.h"
#include "Projectile.h"   // Bullet
#include "Particles.h"    // DeathParticle

#include <nlohmann/json.hpp>

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

constexpr int SAVE_VERSION = 3;
extern int itemCost[6];     // defined in Shop.cpp; saved so shop prices match your upgrades


namespace SaveLimits
{
    constexpr std::size_t MAX_ENEMIES = 1000;
    constexpr std::size_t MAX_BULLETS = 10000;    // for each of the two bullet lists
    constexpr std::size_t MAX_PARTICLES = 20000;
    constexpr long long   MAX_FILE_BYTES = 16LL * 1024 * 1024;

    constexpr float MAX_COORD = 100000.0f;         // positions
    constexpr float MAX_RADIUS = 1000.0f;
    constexpr float MAX_SPEED = 100000.0f;         // velocities
    constexpr float MAX_EXP = 1000000000.0f;
    constexpr int   MAX_STAT = 1000000000;        // health, damage, level, score ...
}

// ---------------------------------------------------------------------------
// The result of a save or load
// ---------------------------------------------------------------------------

struct SaveResult
{
    bool        ok = true;
    std::string message;   // what went wrong (empty when ok)

    explicit operator bool() const { return ok; }
};

inline SaveResult Fail(const std::string& why)
{
    SaveResult r;
    r.ok = false;
    r.message = why;
    return r;
}

// ---------------------------------------------------------------------------
// WRITING: game objects -> JSON
// ---------------------------------------------------------------------------

inline void to_json(json& j, const Vector2& v)
{
    j = json{ {"x", v.x}, {"y", v.y} };
}

inline void to_json(json& j, const Color& c)
{
    j = json{ {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

// Saved as text so the file is easy to read and edit. If you add an EnemyType,
// add it here AND in ReadEnemyType below.
inline const char* EnemyTypeName(EnemyType t)
{
    switch (t)
    {
    case EnemyType::Melee:    return "Melee";
    case EnemyType::Square:   return "Square";
    case EnemyType::Ranged:   return "Ranged";
    case EnemyType::Triangle: return "Triangle";
    case EnemyType::MiniBossH:  return "MiniBossH";
    case EnemyType::MiniBossSp: return "MiniBossSp";
    }
    return "Unknown";
}

inline void to_json(json& j, const Player& p)
{
    j = json{
    {"pos",            p._pos},
    {"health",         p.health},
    {"maxHealth",      p.maxHealth},
    {"level",          p.level},
    {"enemyAmt",       p.EnemyAmt},
    {"damage",         p.damage},
    {"playerSpeed",    p.playerSpeed},
    {"exp",            p._exp},
    {"requiredExp",    p.requiredExp},
    {"shield",         p.shield},
    {"shieldMax",      p.shieldMax},
    {"shieldCooldown", p.shieldCooldownTimer},
    {"waveCooldown",   p.waveCooldownTimer},
    {"extraLife",      p.hasExtraLife},
    {"tripleShot",     p.tripleShotActive},
    {"rapidLevel",     p.rapidLevel},
    {"points",         p.points},
    {"shopCosts",      std::vector<int>(itemCost, itemCost + 6)}
    };
}

inline void to_json(json& j, const AI_Agent& a)
{
    j = json{
        {"type",      EnemyTypeName(a.type)},
        {"pos",       a._Pos},
        {"radius",    a._radius},
        {"health",    a.Health},
        {"maxHealth", a.maxHealth},
        {"maxSpeed",  a.maxSpeed}
    };
}

inline void to_json(json& j, const Bullet& b)
{
    j = json{
        {"pos",    b._pos},
        {"vel",    b._vel},
        {"radius", b._radius},
        {"colour", b._colour},
		{"damage", b.damage}
    };
}

inline void to_json(json& j, const DeathParticle& d)
{
    j = json{
        {"pos",         d.position},
        {"vel",         d.velocity},
        {"lifetime",    d.lifetime},
        {"maxLifetime", d.maxLifetime},
        {"radius",      d.radius},
        {"colour",      d.color}
    };
}

inline json BuildSaveJson(const Player& player,
    const std::vector<AI_Agent>& enemies,
    const std::vector<Bullet>& bullets,
    const std::vector<Bullet>& enemyBullets,
    const std::vector<DeathParticle>& particles,
    int score)
{
    json root;
    root["version"] = SAVE_VERSION;
    root["score"] = score;
    root["player"] = player;
    root["enemies"] = enemies;
    root["bullets"] = bullets;
    root["enemyBullets"] = enemyBullets;
    root["particles"] = particles;
    return root;
}

// ---------------------------------------------------------------------------
// READING: JSON -> game objects, checking everything on the way
//
// Each small helper below reads ONE value and throws a SaveError that says
// exactly what is wrong. The Parse... functions use them to read whole objects.
// ---------------------------------------------------------------------------

namespace save_detail
{
    class SaveError : public std::runtime_error
    {
    public:
        explicit SaveError(const std::string& message) : std::runtime_error(message) {}
    };

    inline std::string Quote(const char* key)
    {
        return std::string("\"") + key + "\"";
    }

    // A short printable copy of a value, for error messages.
    inline std::string Show(const json& v)
    {
        if (v.is_number_float() && !std::isfinite(v.get<double>()))
            return "NaN or infinity";

        std::string s = v.dump();
        if (s.size() > 40) s = s.substr(0, 37) + "...";
        return s;
    }

    // Finds "key" inside an object, or throws.
    inline const json& Field(const json& j, const char* key)
    {
        if (!j.is_object())
            throw SaveError(std::string("expected an object but found ") + j.type_name());

        json::const_iterator it = j.find(key);
        if (it == j.end())
            throw SaveError("missing " + Quote(key));

        return *it;
    }

    // Reads a number (int or float) and checks lo <= value <= hi.
    template <class T>
    T Number(const json& j, const char* key, T lo, T hi)
    {
        const json& v = Field(j, key);

        const bool wholeNumber = std::is_integral<T>::value;
        if (wholeNumber ? !v.is_number_integer() : !v.is_number())
            throw SaveError(Quote(key) + (wholeNumber ? " must be a whole number, found "
                : " must be a number, found ") + Show(v));

        const double d = v.get<double>();
        if (!(d >= lo && d <= hi))                 // written this way so NaN also fails
            throw SaveError(Quote(key) + " is out of range: " + Show(v));

        return static_cast<T>(d);
    }

    inline Vector2 ReadVec2(const json& j, const char* key, float limit)
    {
        const json& v = Field(j, key);
        try
        {
            Vector2 out = { Number<float>(v, "x", -limit, limit),
                            Number<float>(v, "y", -limit, limit) };
            return out;
        }
        catch (const SaveError& e)
        {
            throw SaveError(Quote(key) + ": " + e.what());
        }
    }

    inline Color ReadColor(const json& j, const char* key)
    {
        const json& v = Field(j, key);
        try
        {
            Color c = { static_cast<unsigned char>(Number<int>(v, "r", 0, 255)),
                        static_cast<unsigned char>(Number<int>(v, "g", 0, 255)),
                        static_cast<unsigned char>(Number<int>(v, "b", 0, 255)),
                        static_cast<unsigned char>(Number<int>(v, "a", 0, 255)) };
            return c;
        }
        catch (const SaveError& e)
        {
            throw SaveError(Quote(key) + ": " + e.what());
        }
    }

    // Reads a true/false value, or throws.
    inline bool ReadBool(const json& j, const char* key)
    {
        const json& v = Field(j, key);
        if (!v.is_boolean()) throw SaveError(Quote(key) + " must be true or false, found " + Show(v));
        return v.get<bool>();
    }

    inline EnemyType ReadEnemyType(const json& j)
    {
        const json& v = Field(j, "type");
        if (v.is_string())
        {
            const std::string s = v.get<std::string>();
            if (s == "Melee")    return EnemyType::Melee;
            if (s == "Square")   return EnemyType::Square;
            if (s == "Ranged")   return EnemyType::Ranged;
            if (s == "Triangle") return EnemyType::Triangle;
            if (s == "MiniBossH")  return EnemyType::MiniBossH;
            if (s == "MiniBossSp") return EnemyType::MiniBossSp;
        }
        throw SaveError("\"type\" must be \"Melee\", \"Square\", \"Ranged\" or \"Triangle\", found " + Show(v));
    }

    // ----- one Parse function per kind of object -----

    // Only the saved player values. LoadGame copies them onto your real Player,
    struct PlayerData
    {
        Vector2 pos = { 0.0f, 0.0f };
        float health = 1.0f, maxHealth = 1.0f;
        int   level = 0, enemyAmt = 3;
        float damage = 1.0f, playerSpeed = 10.0f;
        float exp = 0.0f, requiredExp = 100.0f;
        float shield = 0.0f, shieldMax = 1.0f, shieldCooldown = 0.0f, waveCooldown = 0.0f;
        bool  extraLife = false, tripleShot = false;
        int   rapidLevel = 0, points = 0;
        int   shopCosts[6] = {};
    };

    inline PlayerData ParsePlayer(const json& j)
    {
        using namespace SaveLimits;

        PlayerData p;
        p.pos = ReadVec2(j, "pos", MAX_COORD);
        p.maxHealth = Number<float>(j, "maxHealth", 1.0f, (float)MAX_STAT);
        p.health = Number<float>(j, "health", 0.0001f, (float)MAX_STAT);   // 0 = dead, never saved
        if (p.health > p.maxHealth)
            throw SaveError("\"health\" is higher than \"maxHealth\"");
        p.level = Number<int>(j, "level", 0, MAX_STAT);
        p.enemyAmt = Number<int>(j, "enemyAmt", 1, static_cast<int>(MAX_ENEMIES));
        p.damage = Number<float>(j, "damage", 0.1f, (float)MAX_STAT);
        p.playerSpeed = Number<float>(j, "playerSpeed", 0.0f, 1000.0f);
        p.exp = Number<float>(j, "exp", 0.0f, MAX_EXP);
        p.requiredExp = Number<float>(j, "requiredExp", 1.0f, MAX_EXP);

        p.shieldMax = Number<float>(j, "shieldMax", 0.0f, 1000.0f);
        p.shield = Number<float>(j, "shield", 0.0f, 1000.0f);
        if (p.shield > p.shieldMax)
            throw SaveError("\"shield\" is higher than \"shieldMax\"");
        p.shieldCooldown = Number<float>(j, "shieldCooldown", 0.0f, Balance::SHIELD_COOLDOWN);
        p.waveCooldown = Number<float>(j, "waveCooldown", 0.0f, Balance::WAVE_CLEAR_COOLDOWN);

        p.extraLife = ReadBool(j, "extraLife");
        p.tripleShot = ReadBool(j, "tripleShot");
        p.rapidLevel = Number<int>(j, "rapidLevel", 0, 100);
        p.points = Number<int>(j, "points", 0, MAX_STAT);

        const json& costs = Field(j, "shopCosts");
        if (!costs.is_array() || costs.size() != 6)
            throw SaveError("\"shopCosts\" must be a list of 6 whole numbers");
        for (int i = 0; i < 6; ++i)
        {
            if (!costs[i].is_number_integer() || costs[i].get<int>() < 1 || costs[i].get<int>() > MAX_STAT)
                throw SaveError("\"shopCosts\" has a bad value at position " + std::to_string(i));
            p.shopCosts[i] = costs[i].get<int>();
        }
        return p;
    }

    inline AI_Agent ParseEnemy(const json& j)
    {
        using namespace SaveLimits;

        AI_Agent a;
        a.type = ReadEnemyType(j);
        a._Pos = ReadVec2(j, "pos", MAX_COORD);
        a._radius = Number<float>(j, "radius", 1.0f, MAX_RADIUS);
        a.maxHealth = Number<float>(j, "maxHealth", 1.0f, (float)MAX_STAT);
        a.Health = Number<float>(j, "health", 0.0001f, (float)MAX_STAT);          // 0 = dead, deleted every frame
        if (a.Health > a.maxHealth)
            throw SaveError("\"health\" (" + std::to_string(a.Health) +
                ") is higher than \"maxHealth\" (" + std::to_string(a.maxHealth) + ")");
        a.maxSpeed = Number<float>(j, "maxSpeed", 0.0f, MAX_SPEED);
        a.active = true;        // not saved: every saved enemy is alive
        a.expAmount = 0.0f;        // unused by the game, but the constructor leaves it uninitialised
        return a;
    }

    inline Bullet ParseBullet(const json& j)
    {
        using namespace SaveLimits;

        Bullet b;
        b._pos = ReadVec2(j, "pos", MAX_COORD);
        b._vel = ReadVec2(j, "vel", MAX_SPEED);
        b._radius = Number<float>(j, "radius", 1.0f, MAX_RADIUS);
        b._colour = ReadColor(j, "colour");
        b.damage = Number<float>(j, "damage", 0.0f, (float)MAX_STAT);
        b._disabled = false;       // not saved: disabled bullets are deleted every frame
        return b;
    }

    inline DeathParticle ParseParticle(const json& j)
    {
        using namespace SaveLimits;

        DeathParticle d;
        d.position = ReadVec2(j, "pos", MAX_COORD);
        d.velocity = ReadVec2(j, "vel", MAX_SPEED);
        d.lifetime = Number<float>(j, "lifetime", 0.0f, 60.0f);
        d.maxLifetime = Number<float>(j, "maxLifetime", 0.01f, 60.0f);   // never 0: the game divides by it
        d.radius = Number<float>(j, "radius", 0.1f, MAX_RADIUS);
        d.color = ReadColor(j, "colour");
        return d;
    }

    // Reads a whole list. A problem in entry 3 is reported as "enemies[3]: ...".
    template <class T>
    std::vector<T> ReadList(const json& root, const char* key, std::size_t maxCount,
        T(*parseOne)(const json&))
    {
        const json& list = Field(root, key);
        if (!list.is_array())
            throw SaveError(Quote(key) + " must be a list, found " + list.type_name());
        if (list.size() > maxCount)
            throw SaveError(Quote(key) + " has too many entries (" + std::to_string(list.size()) +
                ", the limit is " + std::to_string(maxCount) + ")");

        std::vector<T> out;
        out.reserve(list.size());
        for (std::size_t i = 0; i < list.size(); ++i)
        {
            try
            {
                out.push_back(parseOne(list.at(i)));
            }
            catch (const SaveError& e)
            {
                throw SaveError(std::string(key) + "[" + std::to_string(i) + "]: " + e.what());
            }
        }
        return out;
    }

    // A whole save file, fully checked.
    struct SaveContents
    {
        PlayerData                 player;
        std::vector<AI_Agent>      enemies;
        std::vector<Bullet>        bullets;
        std::vector<Bullet>        enemyBullets;
        std::vector<DeathParticle> particles;
        int                        score = 0;
    };

    inline SaveContents ParseSave(const json& root)
    {
        using namespace SaveLimits;

        if (!root.is_object())
            throw SaveError(std::string("the save file must contain a JSON object, found ") + root.type_name());

        // ----- version -----
        const int version = Number<int>(root, "version", 0, 1000000);
        if (version < SAVE_VERSION)
            throw SaveError("this save is from an older version of the game (save version " +
                std::to_string(version) + ", the game now uses " + std::to_string(SAVE_VERSION) + ")");
        if (version > SAVE_VERSION)
            throw SaveError("this save was made by a newer version of the game (save version " +
                std::to_string(version) + ", the game understands " + std::to_string(SAVE_VERSION) + ")");

        // ----- contents -----
        SaveContents c;

        const json& playerJson = Field(root, "player");
        try
        {
            c.player = ParsePlayer(playerJson);
        }
        catch (const SaveError& e)
        {
            throw SaveError(std::string("player: ") + e.what());
        }

        c.enemies = ReadList<AI_Agent>(root, "enemies", MAX_ENEMIES, ParseEnemy);
        c.bullets = ReadList<Bullet>(root, "bullets", MAX_BULLETS, ParseBullet);
        c.enemyBullets = ReadList<Bullet>(root, "enemyBullets", MAX_BULLETS, ParseBullet);
        c.particles = ReadList<DeathParticle>(root, "particles", MAX_PARTICLES, ParseParticle);
        c.score = Number<int>(root, "score", 0, MAX_STAT);
        return c;
    }
}

// ---------------------------------------------------------------------------
// SAVE
// ---------------------------------------------------------------------------

inline SaveResult SaveGame(const std::string& path,
    const Player& player,
    const std::vector<AI_Agent>& enemies,
    const std::vector<Bullet>& bullets,
    const std::vector<Bullet>& enemyBullets,
    const std::vector<DeathParticle>& particles,
    int score)
{
    std::string text;
    try
    {
        json root = BuildSaveJson(player, enemies, bullets, enemyBullets, particles, score);

        // Would the game accept this file? If not (dead player, NaN, ...), do not write it.
        save_detail::ParseSave(root);

        text = root.dump(2);   // 2 = readable formatting
    }
    catch (const save_detail::SaveError& e)
    {
        return Fail(std::string("refusing to save: ") + e.what());
    }
    catch (const std::exception& e)
    {
        return Fail(std::string("could not build the save: ") + e.what());
    }

    // Write to a temporary file first, so a crash or full disk can never leave
    // a half-written save.json behind.
    const std::string tmpPath = path + ".tmp";
    {
        std::ofstream out(tmpPath.c_str(), std::ios::binary | std::ios::trunc);
        if (!out)
            return Fail("could not create " + tmpPath);

        out.write(text.data(), static_cast<std::streamsize>(text.size()));
        out.flush();
        if (!out.good())
        {
            out.close();
            std::remove(tmpPath.c_str());
            return Fail("could not write " + tmpPath + " (disk full?)");
        }
    }

    // On Windows rename() will not overwrite, so delete the old save first.
    // If the game dies in this tiny gap, the complete new save is still in the .tmp file.
    std::remove(path.c_str());
    if (std::rename(tmpPath.c_str(), path.c_str()) != 0)
        return Fail("could not replace " + path + " (the new save is in " + tmpPath + ")");

    return SaveResult();
}

// ---------------------------------------------------------------------------
// LOAD
// Your game is only touched if the WHOLE file is good.
// ---------------------------------------------------------------------------

inline SaveResult LoadGame(const std::string& path,
    Player& player,
    std::vector<AI_Agent>& enemies,
    std::vector<Bullet>& bullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<DeathParticle>& particles,
    int& score)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        return Fail("could not open " + path + " (has the game been saved yet?)");

    // Look at the size first, so a huge wrong file cannot eat all the memory.
    file.seekg(0, std::ios::end);
    const std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (fileSize == 0)
        return Fail("the save file is empty");
    if (fileSize < 0 || fileSize > SaveLimits::MAX_FILE_BYTES)
        return Fail("the save file is too large to be a save (limit 16 MB)");

    json root = json::parse(file, nullptr, false);   // false = do not throw on bad JSON
    if (root.is_discarded())
        return Fail("the save file is corrupt or incomplete (it is not valid JSON)");

    save_detail::SaveContents loaded;
    try
    {
        loaded = save_detail::ParseSave(root);
    }
    catch (const save_detail::SaveError& e)
    {
        return Fail(e.what());
    }
    catch (const std::exception& e)
    {
        return Fail(std::string("unexpected problem reading the save: ") + e.what());
    }

    // Everything checked out: only now change the real game.
    const save_detail::PlayerData& d = loaded.player;
    
    player._pos = d.pos;
    player.health = d.health;
    player.maxHealth = d.maxHealth;
    player.level = d.level;
    player.EnemyAmt = d.enemyAmt;
    player.damage = d.damage;
    player.playerSpeed = d.playerSpeed;
    player._exp = d.exp;
    player.requiredExp = d.requiredExp;
    player.shieldMax = d.shieldMax;
    player.shield = d.shield;
    player.shieldCooldownTimer = d.shieldCooldown;
    player.waveCooldownTimer = d.waveCooldown;
    player.hasExtraLife = d.extraLife;
    player.tripleShotActive = d.tripleShot;
    player.rapidLevel = d.rapidLevel;
    player.points = d.points;
    for (int i = 0; i < 6; ++i) itemCost[i] = d.shopCosts[i];

    player.dead = false;
    player.levelUp = false;
    player.bulletsToSpawn = 0;

    enemies = std::move(loaded.enemies);
    bullets = std::move(loaded.bullets);
    enemyBullets = std::move(loaded.enemyBullets);
    particles = std::move(loaded.particles);
    score = loaded.score;
    return SaveResult();
}