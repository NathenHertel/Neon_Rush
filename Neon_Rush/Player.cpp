#include "Player.h"
#include "Assets.h"

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
	if (IsKeyDown(KEY_RIGHT)) _pos.x += speed;
	else if (IsKeyDown(KEY_D)) _pos.x += speed;

	// ----- LEFT -----
	if (IsKeyDown(KEY_LEFT)) _pos.x -= speed;
	else if (IsKeyDown(KEY_A)) _pos.x -= speed;

	// ----- UP -----
	if (IsKeyDown(KEY_UP)) _pos.y -= speed;
	else if (IsKeyDown(KEY_W)) _pos.y -= speed;

	// ----- DOWN -----
	if (IsKeyDown(KEY_DOWN)) _pos.y += speed;
	else if (IsKeyDown(KEY_S)) _pos.y += speed;

	_pos.x = Clamp(_pos.x, _radius, screenWidth - _radius);
	_pos.y = Clamp(_pos.y, _radius, screenHeight - _radius);
}

void Player::Reset()
{
	_pos = { screenWidth / 2, screenHeight / 2 };
	health = baseHealth;
	maxHealth = baseHealth;
	levelUp = false;
	Level = 0;
	EnemyAmt = 5;
	_exp = 0.0f;
	requiredExp = 100.0f;
	fireInterval = 1.0f;
	damage = DmgBase;
}