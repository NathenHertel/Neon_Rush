#include "EnemySpawning.h"

EnemySpawner::EnemySpawner(float interval_, int maxEnemies_) 
	: interval(interval_), maxEnemies(maxEnemies_)
{

}

void EnemySpawner::Update(float deltaTime, std::vector<AI_Agent>& enemies, std::mt19937& gen)
{
	timerCircle += deltaTime;
	timerSquare += deltaTime;

	if (timerCircle >= interval && enemies.size() < maxEnemies)
	{
		enemies.push_back(MakeEnemy(gen));
		timerCircle = 0.0f;
	}
}

// EnemySpawner.cpp
AI_Agent EnemySpawner::MakeEnemy(std::mt19937& gen)
{
	AI_Agent agent;

	agent.SetPos({ disX(gen), disY(gen) });
	agent.type = static_cast<EnemyType>(typeChance(gen));

	switch (agent.type)
	{
	case EnemyType::Melee:
		agent.Health = 10;
		agent.maxSpeed = 300.0f;
		break;
	case EnemyType::Square:
		agent.Health = 15;
		break;
	case EnemyType::Ranged:
		agent.Health = 6;
		agent.maxSpeed = 100.0f;
		break;
	case EnemyType::Triangle:
		agent.Health = 8;
		agent.maxSpeed = 200.0f;
		break;
	}

	agent.maxHealth = agent.Health;
	agent._radius = 20.0f;
	agent.active = true;
	return agent;
}