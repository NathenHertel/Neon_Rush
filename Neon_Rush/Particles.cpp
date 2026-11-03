#include "Particles.h"
#include "raymath.h"

void SpawnDeathParticles(std::vector<DeathParticle>& particles, Vector2 center,
    std::mt19937& rng)
{
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    std::uniform_real_distribution<float> speedDist(PARTICLE_MIN_SPEED, PARTICLE_MAX_SPEED);
    std::uniform_real_distribution<float> lifeDist(0.35f, 0.95f);
    std::uniform_real_distribution<float> radiusDist(1.5f, 4.5f);
    const Color colors[] = { RED,BLUE,GREEN,ORANGE,PURPLE,YELLOW };
    std::uniform_int_distribution<int> colorDist(0, 5);

    for (int i = 0; i < PARTICLES_PER_KILL; ++i)
    {
        const float angle = angleDist(rng);
        const float speed = speedDist(rng);
        const float life = lifeDist(rng);
        particles.push_back({ center,
            { cosf(angle) * speed, sinf(angle) * speed },
            life, life, radiusDist(rng), colors[colorDist(rng)] });
    }
}

void UpdateDeathParticles(std::vector<DeathParticle>& particles, float dt)
{
    for (auto& p : particles)
    {
        p.lifetime -= dt;
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        // Frame-rate independent slowdown.
        const float drag = expf(-PARTICLE_DRAG * dt);
        p.velocity.x *= drag;
        p.velocity.y *= drag;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const DeathParticle& p) { return p.lifetime <= 0.0f; }), particles.end());
}

void DrawDeathParticles(const std::vector<DeathParticle>& particles)
{
    BeginBlendMode(BLEND_ADDITIVE);
    for (const auto& p : particles)
    {
        const float fade = Clamp(p.lifetime / p.maxLifetime, 0.0f, 1.0f);
        const float r = p.radius * (0.25f + 0.75f * fade);
        // Large transparent halo plus a bright center creates a neon glow.
        DrawCircleV(p.position, r * 3.0f, Fade(p.color, 0.12f * fade));
        DrawCircleV(p.position, r * 1.7f, Fade(p.color, 0.30f * fade));
        DrawCircleV(p.position, r, Fade(p.color, fade));
    }
    EndBlendMode();
}