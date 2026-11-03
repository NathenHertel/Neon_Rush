#pragma once

#include "raylib.h"
#include "raymath.h"
#include "Projectile.h"

#include <cmath>
#include <iostream>
#include <vector>

class AI_Agent
{
public:
	// ----- DECLERATIONS -----
	float maxSpeed{ 50.0f };
	Vector2 _Pos{ 0.0f, 0.0f };
	float _radius{ 5.0f };
	Color _colour{ BLUE };
	bool active;

	int Health = 10;
	int maxHealth = 10;
	static constexpr float baseHealth = 10.0f;

	float minExp = 15.0f;
	float maxExp = 50.0f;

	int EnemiesToSpawn = 5;
	static constexpr int EnemyAmtBase = 5;

	float expAmount;

	float shootTimer = 1.5f;
	float shootInterval = 1.5f;

	bool ranged = false;

	AI_Agent();
	AI_Agent(Vector2 pos, float radius, Color colour);

	virtual ~AI_Agent() {};

	static void Update(float deltaTime, Vector2 playerPos, std::vector<AI_Agent>& Enemies, std::vector<Bullet>& EnemyBullets);
	virtual void Draw(Texture texture, int screenWidth, int screenHeight);

	void SetPos(Vector2 pos);
	Vector2 GetPos();

	void setHealth(int dmg);

	void MoveToPlayer(Vector2& targetPos, float speed, float deltaTime);
	void ShootAtPlayer(Vector2& targetPos, float deltaTime, std::vector<Bullet>& EnemyBullets);

	bool CheckOverlapWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius);

	void CollisionWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius);

private:

};