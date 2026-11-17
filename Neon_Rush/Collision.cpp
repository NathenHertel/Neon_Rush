#include "Collision.h"

void PlayerProjectileCheck(std::vector<Bullet>& enemyBullets, Player& player, bool& _gameOver, float deltaTime)
{
    for (auto& b : enemyBullets)
    {
        b.Update(deltaTime, screenWidth, screenHeight);

        if (!b._disabled && CheckCollisionCircles(b._pos, b._radius, player._pos, player._radius))
        {

            // ----- Disable Enemy Bullet -----
            b._disabled = true;

            // ----- Deal Damage To Player -----
            // & check if player is dead
            player.setHealth(3);
            if (player.health <= 0)
            {
                _gameOver = true;
            }
        }
    }
}

void EnemyProjectileCheck(std::vector<Bullet>& bullets, std::vector<AI_Agent>& enemies,
    std::vector<DeathParticle>& particles, Player& player, int& _score, std::mt19937& gen, float dmg, Sound& EnemyDeath)
{
    // ----- Check once for all bullets -----
    for (size_t i = 0; i < bullets.size(); i++)
    {
        // ----- Ignore Bullet if its not active -----
        if (bullets[i]._disabled) continue;

        // ----- Check once for all enemies -----
        for (size_t j = 0; j < enemies.size(); j++)
        {
            // ----- Ignore Enemy if not active -----
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
                PlaySound(EnemyDeath);

                std::uniform_real_distribution<float> distrib(enemies[j].minExp, enemies[j].maxExp);
                LvlUp(&player, distrib(gen));

                switch (enemies[j].type)
                {
                case EnemyType::Melee:    _score += 10; break;
                case EnemyType::Square:   _score += 20; break;
                case EnemyType::Ranged:   _score += 20; break;
                case EnemyType::Triangle: _score += 30; break;
                }
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