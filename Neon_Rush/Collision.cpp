#include "Collision.h"

void PlayerProjectileCheck(std::vector<Bullet>& enemyBullets, Player& player, bool& _gameOver, float deltaTime)
{
    for (auto& b : enemyBullets)
    {
        b.Update(deltaTime, screenWidth, screenHeight);

        if (!b._disabled && CheckCollisionCircles(b._pos, b._radius, player._pos, player._radius))
        {
            b._disabled = true;
            player.setHealth(3);
            if (player.health <= 0)
            {
                _gameOver = true;
            }
        }
    }
}

void EnemyProjectileCheck(std::vector<Bullet>& bullets, std::vector<AI_Agent>& enemies,
    std::vector<DeathParticle>& particles, Player& player, int& _score, std::mt19937& gen, float dmg)
{
    for (size_t i = 0; i < bullets.size(); i++)
    {
        if (bullets[i]._disabled) continue;

        for (size_t j = 0; j < enemies.size(); j++)
        {
            if (!enemies[j].active) continue;

            if (!enemies[j].CheckOverlapWithOtherAgent(enemies[j]._Pos, bullets[i]._pos,
                enemies[j]._radius, bullets[i]._radius))
                continue;

            bullets[i]._disabled = true;
            enemies[j].setHealth(dmg);

            if (enemies[j].Health <= 0)
            {
                SpawnDeathParticles(particles, enemies[j].GetPos(), gen);
                enemies[j].active = false;

                std::uniform_real_distribution<float> distrib(enemies[j].minExp, enemies[j].maxExp);
                LvlUp(&player, distrib(gen));

                _score += enemies[j].ranged ? 2 : 1;
            }
            break;
        }
    }
}

void EnemyDamage(bool& gameOver, std::vector<AI_Agent>& enemies, Player& player, Sound& enemyVoice)
{
    for (size_t i = 0; i < enemies.size(); ++i)
    {
        AI_Agent& a = enemies[i];

        Vector2 AIpos1 = a.GetPos();

        if (a.CheckOverlapWithOtherAgent(AIpos1, player._pos, a._radius, player._radius))
        {
            player.setHealth(3);

            if (player.health <= 0)
            {
                PlaySound(enemyVoice);
                gameOver = true;

                break;
            }
        }
    }
}

void ResetCollision(std::vector<AI_Agent>& enemies, std::vector<Bullet>& bullets, std::vector<Bullet>& enemyBullets)
{
    enemies.erase
    (
        std::remove_if(enemies.begin(), enemies.end(), [](const AI_Agent& e) { return !e.active; }),
        enemies.end()
    );

    bullets.erase
    (
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b._disabled; }),
        bullets.end()
    );

    enemyBullets.erase
    (
        std::remove_if(enemyBullets.begin(), enemyBullets.end(), [](const Bullet& b) { return b._disabled; }),
        enemyBullets.end()
    );
}