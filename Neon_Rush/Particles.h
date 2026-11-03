#pragma once
#include "raylib.h"

#include <vector>
#include <random>

// ---------- ENEMY DEATH PARTICLES ----------
struct DeathParticle
{
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float maxLifetime;
    float radius;
    Color color;
};

// Tweak these to experiment with the explosion effect.
constexpr int PARTICLES_PER_KILL = 80;
constexpr float PARTICLE_MIN_SPEED = 90.0f;
constexpr float PARTICLE_MAX_SPEED = 420.0f;
constexpr float PARTICLE_DRAG = 1.4f;

void SpawnDeathParticles(std::vector<DeathParticle>& particles, Vector2 center, std::mt19937& rng);
void UpdateDeathParticles(std::vector<DeathParticle>& particles, float dt);
void DrawDeathParticles(const std::vector<DeathParticle>& particles);