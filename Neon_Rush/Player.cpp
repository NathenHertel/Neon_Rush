#include "Player.h"
#include "Assets.h"
#include "Shop.h"
#include "GamepadInput.h"

void Player::setHealth(int damage)
{
	health = health - damage;
}

void Player::UpdateAim(Vector2 mousePos)
{
	// ---------- CREATE / ROTATE TRIANGLE ----------
	Vector2 o1 = { 0.0f, -50.0f };
	Vector2 o2 = { -25.0f, 20.0f };
	Vector2 o3 = { 25.0f, 20.0f };

	Vector2 c = { (o1.x + o2.x + o3.x) / 3.0f,
				  (o1.y + o2.y + o3.y) / 3.0f };

	pivot = { _pos.x + c.x, _pos.y + c.y };

	float rad = atan2f(mousePos.y - pivot.y, mousePos.x - pivot.x) + PI / 2.0f;

	auto rot = [&](Vector2 o)
		{
			float dx = o.x - c.x;
			float dy = o.y - c.y;

			return Vector2
			{
				pivot.x + dx * cosf(rad) - dy * sinf(rad),
				pivot.y + dx * sinf(rad) + dy * cosf(rad)
			};
		};

	v1 = rot(o1);
	v2 = rot(o2);
	v3 = rot(o3);
}

void Player::TakeDamage(float dmg)
{
	if (dead || dmg <= 0.0f) return;

	if (shield > 0.0f)
	{
		float absorbed = std::min(shield, dmg);
		shield -= absorbed;
		dmg -= absorbed;
		if (shield <= 0.0001f)
		{
			shield = 0.0f;
			shieldCooldownTimer = Balance::SHIELD_COOLDOWN;   // 60s recharge starts
		}
	}

	if (dmg > 0.0f)
	{
		health -= dmg;
		if (health <= 0.0f)
		{
			if (hasExtraLife) { health = maxHealth; hasExtraLife = false; }
			else { health = 0.0f; dead = true; }
		}
	}
}

bool Player::TryWaveClear()
{
	if (waveCooldownTimer > 0.0f) return false;
	waveCooldownTimer = Balance::WAVE_CLEAR_COOLDOWN;
	return true;
}

void Player::PowerUpdate(float deltaTime)
{
	// permanent buffs
	fireInterval = std::max(Balance::MIN_FIRE_INTERVAL, baseFireInterval * powf(0.9f, (float)rapidLevel));
	//shotCount = tripleShotActive ? 3 : 1;
	shotCount = 1 + 2 * tripleLevel;

	// shield recharge
	if (shieldCooldownTimer > 0.0f)
	{
		shieldCooldownTimer -= deltaTime;
		if (shieldCooldownTimer <= 0.0f) { shieldCooldownTimer = 0.0f; shield = shieldMax; }
	}

	// wave clear cooldown
	if (waveCooldownTimer > 0.0f)
		waveCooldownTimer = std::max(0.0f, waveCooldownTimer - deltaTime);
}

void Player::StartBurst(float& fireTimer)
{
	if (amountOfShots == 0)
	{
		bulletsToSpawn = masterAmount;
		fireTimer = delayBetweenShots;
	}
	else if (amountOfShots > 0)
	{
		bulletsToSpawn = amountOfShots;
		fireTimer = delayBetweenShots;
	}
}

void Player::UpdateShooting(Vector2 mousePos, std::vector<Bullet>& bullets, float deltaTime, float& fireTimer, int screenWidth, int screenHeight)
{
	if (bulletsToSpawn > 0 && fireTimer >= delayBetweenShots)
	{
		float dx = mousePos.x - pivot.x;
		float dy = mousePos.y - pivot.y;

		float length = sqrtf(dx * dx + dy * dy);

		if (length > 0.0f)
		{
			float baseAngle = atan2f(dy, dx);

			// ---- fan goes here, replacing the old SpawnBullet(bullets, baseAngle, 500.0f) ----
			float spread = 15.0f * DEG2RAD;

			for (int i = 0; i < shotCount; i++)
			{
				float offset = (i - (shotCount - 1) / 2.0f) * spread;
				SpawnBullet(bullets, baseAngle + offset, 1200.0f);
			}
		}

		bulletsToSpawn--;
		fireTimer = 0.0f;
	}
}

void Player::DrawPlayer(Texture texture)
{
	Vector2 center = { (v1.x + v2.x + v3.x) / 3.0f,
					   (v1.y + v2.y + v3.y) / 3.0f };

	float angleDeg = atan2f(v1.y - center.y, v1.x - center.x) * RAD2DEG + 90.0f;

	Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
	Rectangle dest = { center.x, center.y, 70.0f, 90.0f };
	Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

	DrawTexturePro(texture, src, dest, origin, angleDeg, WHITE);
}

void Player::HandleMovement(float deltaTime)
{
	// ---------- MOVEMENT ----------

	// ----- RIGHT -----
	if (IsKeyDown(KEY_RIGHT)) _pos.x += playerSpeed;
	else if (IsKeyDown(KEY_D)) _pos.x += playerSpeed;

	// ----- LEFT -----
	if (IsKeyDown(KEY_LEFT)) _pos.x -= playerSpeed;
	else if (IsKeyDown(KEY_A)) _pos.x -= playerSpeed;

	// ----- UP -----
	if (IsKeyDown(KEY_UP)) _pos.y -= playerSpeed;
	else if (IsKeyDown(KEY_W)) _pos.y -= playerSpeed;

	// ----- DOWN -----
	if (IsKeyDown(KEY_DOWN)) _pos.y += playerSpeed;
	else if (IsKeyDown(KEY_S)) _pos.y += playerSpeed;

	// ----- Left stick (analog: a light push moves slower) -----
	Vector2 pad = Pad::Move();
	_pos.x += pad.x * playerSpeed;
	_pos.y += pad.y * playerSpeed;
	
	_pos.x = Clamp(_pos.x, _radius, screenWidth - _radius);
	_pos.y = Clamp(_pos.y, _radius, screenHeight - _radius);
}

void Player::SpawnBullet(std::vector<Bullet>& bullets, float angleRad, float speed)
{
	Bullet b;
	b._pos = v1;                  // tip of the ship
	b._radius = 5.0f;
	b._colour = GREEN; // Set bullet color
	b._vel = { cosf(angleRad) * speed, sinf(angleRad) * speed };
	bullets.push_back(b);
}

void Player::Reset()
{
	_pos = { screenWidth / 2, screenHeight / 2 };
	health = baseHealth;
	maxHealth = baseHealth;
	levelUp = false;
	level = 0;
	EnemyAmt = 5;
	score = 0;
	_exp = 0.0f;
	requiredExp = 100.0f;
	fireInterval = 1.0f;
	damage = DmgBase;
	shieldAmt = 0;
	points = 0;
	rapidFireActive = false;
	rapidFireTimer = 0.0f;
	//tripleShotActive = false;
	tripleLevel = 0;
	tripleShotTimer = 0.0f;
	hasExtraLife = false;
	playerSpeed = 10.0f;
	dead = false; rapidLevel = 0;
	shieldMax = Balance::SHIELD_BASE; shield = shieldMax;
	shieldCooldownTimer = 0.0f; waveCooldownTimer = 0.0f;
}