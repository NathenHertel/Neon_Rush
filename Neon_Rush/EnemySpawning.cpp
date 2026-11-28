#include "EnemySpawning.h"

EnemySpawner::EnemySpawner(float interval_, int maxEnemies_) 
	: interval(interval_), maxEnemies(maxEnemies_)
{

}

void EnemySpawner::ScaleAllEnemies(std::vector<AI_Agent>& enemies, EnemySpawner& spawner)
{
	for (auto& e : enemies)
	{
		if (e.active)
			e.Scaling();
	}
	spawner.scaleCount++;
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

	if (MiniBossSpawn)
	{
		std::uniform_int_distribution<int> pick(0, 1);
		agent.type = (pick(gen) == 0) ? EnemyType::MiniBossH : EnemyType::MiniBossSp;
		MiniBossSpawn = false;
	}
	else
	{
		agent.type = static_cast<EnemyType>(typeChance(gen));
	}

	switch (agent.type)
	{
	case EnemyType::Melee:
		agent.Health = 12.0f;
		agent.damage = 1.0f;
		agent.pointVal = 50;
		agent.maxSpeed = 200.0f;
		agent._radius = 20.0f;
		// No Colour needed
		break;
	case EnemyType::Square:
		agent.Health = 25.0f;
		agent.damage = 3.0f;
		agent.pointVal = 100;
		agent.maxSpeed = 50.0f;
		agent._radius = 20.0f;
		agent._colour = ORANGE;
		break;
	case EnemyType::Ranged:
		agent.Health = 10.0f;
		agent.damage = 1.0f;
		agent.pointVal = 50;
		agent.maxSpeed = 200.0f;
		agent._radius = 20.0f;
		// No Colour needed
		break;
	case EnemyType::Triangle:
		agent.Health = 1.0f;
		agent.damage = 2.0f;
		agent.pointVal = 50;
		agent.maxSpeed = 400.0f;
		agent._radius = 20.0f;
		agent._colour = RED;
		break;
	case EnemyType::MiniBossH:
		agent.Health = 50.0f;
		agent.damage = 5.0f;
		agent.pointVal = 1000;
		agent.maxSpeed = 50.0f;
		agent._radius = 40.0f;
		agent.hasAura = true;
		agent._colour = PURPLE;
		break;
	case EnemyType::MiniBossSp:
		agent.Health = 50.0f;
		agent.damage = 5.0f;
		agent.pointVal = 1000;
		agent.maxSpeed = 400.0f;
		agent._radius = 40.0f;
		agent.hasAura = true;
		agent._colour = SKYBLUE;
		break;
	}

	// for (int i = 0; i < scaleCount; i++)
	// 	agent.Scaling();

	agent.ApplyDifficulty(level);

	agent.active = true;
	return agent;
}