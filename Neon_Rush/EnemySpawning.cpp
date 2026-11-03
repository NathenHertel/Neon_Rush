#include "EnemySpawning.h"

EnemySpawner::EnemySpawner(float interval_, int maxEnemies_) 
	: interval(interval_), maxEnemies(maxEnemies_)
{

}

void EnemySpawner::Update(float deltaTime, std::vector<AI_Agent>& enemies, std::mt19937& gen)
{
	timer += deltaTime;

	if (timer >= interval && enemies.size() < maxEnemies)
	{
		enemies.push_back(MakeEnemy(gen));
		timer = 0.0f;
	}
}

AI_Agent EnemySpawner::MakeEnemy(std::mt19937& gen)
{
	AI_Agent agent;

	agent.ranged = rangedChance(gen);
	agent.SetPos({ disX(gen), disY(gen) });
	agent.maxSpeed = 150.0f;
	agent._radius = 20.0f;
	agent.active = true;

	return agent;
}