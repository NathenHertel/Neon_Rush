#pragma once
#include "Agent.h"
#include "Config.h"
#include "Player.h"

#include <vector>
#include <random>
#include <optional>

class EnemySpawner
{
public:
	int scaleCount = 0;

	void ScaleAllEnemies(std::vector<AI_Agent>& enemies, EnemySpawner& spawner);

	EnemySpawner() = default;
	EnemySpawner(float interval, int maxEnemies);

	void SpawnMiniBoss() { MiniBossSpawn = true; }
	void SpawnBigBoss() { BigBossSpawn = true; }


	void Update(float deltaTime, std::vector<AI_Agent>& enemies, std::mt19937& gen);
	void Reset(int maxEn) 
	{
		timerCircle = 0.0f,
		timerSquare = 0.0f,
		timerTriangle = 0.0f;
		maxEnemies = maxEn;
	};
	void ResetAll(int maxEn)
	{
		timerCircle = 0.0f,
		timerSquare = 0.0f,
		timerTriangle = 0.0f;
		maxEnemies = maxEn,
		scaleCount = 0;
	}

private:
	AI_Agent MakeEnemy(std::mt19937& gen);

	std::discrete_distribution<int> typeChance{ 40, 20, 25, 15 };

	bool MiniBossSpawn = false;
	bool BigBossSpawn = false;

	float timerCircle = 0.0f;
	float timerSquare = 0.0f;
	float timerTriangle = 0.0f;
	float interval = 0.5f;
	int maxEnemies;

	std::uniform_real_distribution<float> disX{ 20.0f, screenWidth - 20.0f };
	std::uniform_real_distribution<float> disY{ 20.0f, screenHeight - 20.0f };
};