#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Projectile.h"

#include <iostream>
#include <vector>

class Player
{
public:
	Vector2 _pos{ 0.0f, 0.0f };

	int health = 9;
	int baseHealth = 9;
	int Level = 0;
	int amountOfShots = 0;
	int bulletsToSpawn = 0;
	int masterAmount = 1;

	bool levelUp = false;

	float _radius = 20.0f;
	float delayBetweenShots = 0.1f;
	float _exp = 0.0f;
	float requiredExp = 100.0f;

	void setHealth(int damage);

	void LevelUp(float exp);

	void StartBurst(float& fireTimer);
	void UpdateShooting(Vector2 mousePos, Vector2 pivotPos,
		std::vector<Bullet>& bullets, Vector2 v1, float deltaTime, float& fireTimer, int screenWidth, int screenHeight);
};