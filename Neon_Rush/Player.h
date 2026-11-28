#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Projectile.h"
#include "Config.h"
#include "Balance.h"

#include <iostream>
#include <vector>

class Player
{
public:
	Vector2 _pos{ 0.0f, 0.0f };
	Vector2 pivot;
	Vector2 v1, v2, v3;

	float health = 9.0f;
	float maxHealth = 9.0f;
	static constexpr float baseHealth = 9.0f;

	float damage = 5.0f;
	static constexpr float DmgBase = 5.0f;

	float playerSpeed = 10.0f;

	bool hasExtraLife = false;

	Texture2D* tex;
	Vector2 texPos;

	int EnemyAmt = 5;

	int level = 0;

	bool rapidFireActive = false;
	bool tripleShotActive = false; //changed to permanent powerup, can be removed if needed
	int tripleLevel = 0;      // 0 = single shot, each level adds 2 bullets

	int score = 0;
	bool levelUp = false;
	int shieldAmt = 0;

	bool  dead = false;
	int   rapidLevel = 0;               // permanent rapid-fire purchases

	float shield = Balance::SHIELD_BASE;
	float shieldMax = Balance::SHIELD_BASE;
	float shieldCooldownTimer = 0.0f;   // > 0 while recharging
	float waveCooldownTimer = 0.0f;     // 0 = ready

	bool spawnMiniBoss = false;

	int points = 0;

	int amountOfShots = 0;
	int shotCount = 1;
	int bulletsToSpawn = 0;
	int masterAmount = 1;

	float rapidFireTimer = 0.0f;
	float tripleShotTimer = 0.0f;
	const float rapidFireDur = 15.0f;
	const float tripleShotDur = 10.0f;

	float fireInterval = 1.0f;
	float baseFireInterval = 1.0f;
	static constexpr float masterFireInt = 1.0f;
	float currentFireInt = 0.0f;
	float delayBetweenShots = 0.1f;

	float _radius = 20.0f;
	float _exp = 0.0f;
	float requiredExp = 100.0f;

	void setHealth(int damage);

	void TakeDamage(float dmg);
	bool TryWaveClear();

	void UpdateAim(Vector2 mousePos);

	void PowerUpdate(float deltaTime);

	void StartBurst(float& fireTimer);
	void UpdateShooting(Vector2 mousePos, std::vector<Bullet>& bullets, float deltaTime, float& fireTimer, int screenWidth, int screenHeight);

	void SpawnBullet(std::vector<Bullet>& bullets, float angleRad, float speed);

	void DrawPlayer(Texture texture);

	void HandleMovement(float deltaTime);

	void Reset();
};