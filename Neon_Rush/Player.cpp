#include "Player.h"

void Player::setHealth(int damage)
{
	health = health - damage;
}

void Player::LevelUp(float exp)
{
	_exp = _exp + exp;

	if (_exp >= requiredExp)
	{
		health = health * 1.5;
		baseHealth = baseHealth * 1.5;
		_exp = _exp - requiredExp;
		requiredExp = requiredExp * 1.5;
		levelUp = true;
	}
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

void Player::UpdateShooting(Vector2 mousePos, Vector2 pivotPos,
	std::vector<Bullet>& bullets, Vector2 v1, float deltaTime, float& fireTimer, int screenWidth, int screenHeight)
{
	if (bulletsToSpawn > 0 && fireTimer >= delayBetweenShots)
	{
		float dx = mousePos.x - pivotPos.x;
		float dy = mousePos.y - pivotPos.y;

		float length = sqrtf(dx * dx + dy * dy);

		if (length > 0.0f)
		{
			Bullet newBullet;
			newBullet._pos.x = v1.x;
			newBullet._pos.y = v1.y;
			newBullet._radius = 5.0f;

			float dirX = dx / length;
			float dirY = dy / length;

			Vector2 vel = { (dirX * 500.0f), (dirY * 500.0f) };

			newBullet._vel = vel;

			newBullet.Update(deltaTime, screenWidth, screenHeight);

			bullets.push_back(newBullet);
		}

		bulletsToSpawn--;
		fireTimer = 0.0f;
	}
}