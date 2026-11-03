#pragma once
#include "Agent.h"
#include "Config.h"
#include "Player.h"

#include <vector>
#include <random>

class EnemySpawner
{
public:
	EnemySpawner() = default;
	EnemySpawner(float interval, int maxEnemies);

	void Update(float deltaTime, std::vector<AI_Agent>& enemies, std::mt19937& gen);
	void Reset(int maxEn) 
	{
		timer = 0.0f,
		maxEnemies = maxEn;
	};

private:
	AI_Agent MakeEnemy(std::mt19937& gen);

	float timer = 0.0f;
	float interval = 0.5f;
	int maxEnemies;

	std::uniform_real_distribution<float> disX{ 20.0f, screenWidth - 20.0f };
	std::uniform_real_distribution<float> disY{ 20.0f, screenHeight - 20.0f };
	std::bernoulli_distribution rangedChance{ 0.5 };
};