#pragma once

#include "raylib.h"
#include "raymath.h"
#include "Projectile.h"

#include <cmath>
#include <iostream>
#include <vector>

// ----- Enum of All possible Enemy Types -----
enum class EnemyType { Melee, Square, Ranged, Triangle, MiniBossH, MiniBossSp };

class AI_Agent
{
public:
	// ----- DECLERATIONS -----
	// --- Floats ---
	float Health{ 10.0f };
	float maxHealth{ 10.0f };
	float damage{ 3.0f };
	float maxSpeed{ 50.0f };
	float _radius{ 5.0f };
	float auraRad{ 10.0f };
	float speedMultiplier{ 1.0f };
	float auraSpeedBoost{ 1.5f };
	float healthMultiplier{ 1.0f };
	float auraHealthBoost{ 5.0f };
	float auraRadius{ 150.0f };
	float minExp{ 15.0f };
	float maxExp{ 50.0f };
	float expAmount{ 0 };
	float shootTimer{ 1.5f };
	float shootInterval{ 1.5f };

	// --- Integers ---
	int pointVal = 0;
	int EnemiesToSpawn = 5;
	static constexpr int EnemyAmtBase = 5;

	// --- Bool ---
	bool active;
	bool hasAura = false;

	// --- Other ---
	Vector2 _Pos{ 0.0f, 0.0f };
	Color _colour{ BLUE };

	// --- Default Enemy Type ---
	EnemyType type = EnemyType::Melee;


	// ----- FUNCTIONS -----
	AI_Agent();
	AI_Agent(Vector2 pos, float radius, Color colour);

	virtual ~AI_Agent() {};

	static void Update(float deltaTime, Vector2 playerPos, std::vector<AI_Agent>& Enemies, std::vector<Bullet>& EnemyBullets);
	virtual void Draw(Texture texture, int screenWidth, int screenHeight);

	virtual void drawAura();

	virtual void DrawSquare();
	virtual void DrawTriangle();

	void SetPos(Vector2 pos);
	Vector2 GetPos();

	void setHealth(float dmg);

	void Scaling();

	void SpeedAura(std::vector<AI_Agent>& Enemies);
	void HealthAura(std::vector<AI_Agent>& Enemies);

	void MoveToPlayer(Vector2& targetPos, float speed, float deltaTime);
	void ShootAtPlayer(Vector2& targetPos, float deltaTime, std::vector<Bullet>& EnemyBullets);

	bool CheckOverlapWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius);

	void CollisionWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius);

private:

};