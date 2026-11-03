#pragma once

#include "Projectile.h"
#include "Agent.h"
#include "Player.h"
#include "Config.h"
#include "Particles.h"
#include "LevelUp.h"

#include "raylib.h"

#include <vector>
#include <random>

void PlayerProjectileCheck(std::vector<Bullet>& enemyBullets, Player& player, bool& _gameOver, float deltaTime);

void EnemyProjectileCheck(std::vector<Bullet>& bullets, std::vector<AI_Agent>& enemies, 
	std::vector<DeathParticle>& particles, Player& player, int& _score, std::mt19937& gen, float dmg);

void EnemyDamage(bool& gameOver, std::vector<AI_Agent>& enemies, Player& player, Sound& enemyVoice);

void ResetCollision(std::vector<AI_Agent>& enemies, std::vector<Bullet>& bullets, std::vector<Bullet>& enemyBullets);