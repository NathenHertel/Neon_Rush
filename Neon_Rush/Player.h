#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Projectile.h"
#include "Config.h"

#include <iostream>
#include <vector>

class Player
{
public:
	Vector2 _pos{ 0.0f, 0.0f };
	Vector2 pivot;
	Vector2 v1, v2, v3;

	int health = 9;
	int maxHealth = 9;
	static constexpr int baseHealth = 9;

	int damage = 5;
	static constexpr int DmgBase = 5;

	Texture2D* tex;
	Vector2 texPos;

	int Level = 0;
	int EnemyAmt = 5;

	int amountOfShots = 0;
	int bulletsToSpawn = 0;
	int masterAmount = 1;

	bool levelUp = false;

	float fireInterval = 0.01f;
	static constexpr float masterFireInt = 1.0f;
	float delayBetweenShots = 0.1f;

	float _radius = 20.0f;
	float _exp = 0.0f;
	float requiredExp = 100.0f;

	void setHealth(int damage);

	void UpdateAim(Vector2 mousePos);

	void StartBurst(float& fireTimer);
	void UpdateShooting(Vector2 mousePos, std::vector<Bullet>& bullets, float deltaTime, float& fireTimer, int screenWidth, int screenHeight);

	void DrawPlayer(Texture texture);

	void HandleMovement(float deltaTime);

	void Reset();
};