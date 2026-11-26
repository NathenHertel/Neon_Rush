#include "Agent.h"

AI_Agent::AI_Agent()
{
	_Pos = Vector2{ 0, 0 };
}

AI_Agent::AI_Agent(Vector2 pos, float radius, Color colour) :
	_Pos{ pos },
	_radius{ radius },
	_colour{ colour },
	active{ true }
{

}

void AI_Agent::SetPos(Vector2 pos)
{
	_Pos = pos;
}

Vector2 AI_Agent::GetPos()
{
	return _Pos;
}

void AI_Agent::Update(float deltaTime, Vector2 playerPos, std::vector<AI_Agent>& Enemies, std::vector<Bullet>& EnemyBullets, Player& _player)
	{

	_player.SpeedMultiplier = 1.0f;

	for (auto& enemy : Enemies)
	{
		enemy.speedMultiplier = 1.0f;
		enemy.healthMultiplier = 1.0f;
	}

	for (auto& enemy : Enemies)
	{
		if (!enemy.active) continue;
		if (enemy.type == EnemyType::MiniBossSp)
			enemy.SpeedAura(Enemies);
		if (enemy.type == EnemyType::MiniBossH)
			enemy.HealthAura(Enemies);
		if (enemy.type == EnemyType::BigBoss)
		{
			enemy.SlowAura(_player);
			enemy.SpeedAura(Enemies);
			enemy.HealthAura(Enemies);
		}
	}

	for (auto& enemy : Enemies)
	{
		if (!enemy.active) continue;

		switch (enemy.type)
		{
		case EnemyType::Ranged:
			enemy.ShootAtPlayer(playerPos, deltaTime, EnemyBullets);
			break;
		case EnemyType::Melee:
		case EnemyType::Square:
		case EnemyType::Triangle:
		case EnemyType::MiniBossH:
		case EnemyType::MiniBossSp:
		case EnemyType::BigBoss:
			enemy.MoveToPlayer(playerPos, enemy.maxSpeed * enemy.speedMultiplier, deltaTime);
			break;
		}
	}

	for (size_t i = 0; i < Enemies.size(); ++i)
	{
		for (size_t j = i + 1; j < Enemies.size(); ++j)
		{
			AI_Agent& a = Enemies[i];
			AI_Agent& b = Enemies[j];

			Vector2 AIpos1 = a.GetPos();
			Vector2 AIpos2 = b.GetPos();

			if (a.CheckOverlapWithOtherAgent(AIpos1, AIpos2, a._radius, b._radius))
			{
				a.CollisionWithOtherAgent(AIpos1, AIpos2, a._radius, b._radius);

				a.SetPos(AIpos1);
				b.SetPos(AIpos2);
			}
		}
	}
}

void AI_Agent::MoveToPlayer(Vector2& targetPos, float speed, float deltaTime)
{
	if (Vector2Distance(_Pos, targetPos) > 2.0f)
	{
		Vector2 Direction = Vector2Normalize(Vector2Subtract(targetPos, _Pos));
		_Pos = Vector2Add(_Pos, Vector2Scale(Direction, speed * deltaTime));
	}
}

void AI_Agent::ShootAtPlayer(Vector2& targetPos, float deltaTime, std::vector<Bullet>& EnemyBullets)
{
	shootTimer += deltaTime;
	if (shootTimer < shootInterval) return;
	if (Vector2Distance(_Pos, targetPos) <= 2.0f) return;

	shootTimer = 0.0f;

	Bullet newBullet;
	Vector2 direction = Vector2Normalize(Vector2Subtract(targetPos, _Pos));

	newBullet._pos = _Pos;
	newBullet._radius = 5.0f;
	newBullet._vel = Vector2Scale(direction, 300.0f);
	newBullet.damage = damage;

	EnemyBullets.push_back(newBullet);
}

bool AI_Agent::CheckOverlapWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius)
{
	float dx = otherPos.x - selfPos.x;
	float dy = otherPos.y - selfPos.y;
	float radiusSum = selfRadius + otherRadius;

	return (dx * dx + dy * dy) < (radiusSum * radiusSum);
}

void AI_Agent::Scaling()
{
	Health *= 1.05f;
	damage *= 1.01f;

	minExp *= 1.01f;
	maxExp *= 1.01f;
}

void AI_Agent::SpeedAura(std::vector<AI_Agent>& Enemies)
{
	for (auto& other : Enemies)
	{
		if (!other.active || &other == this) continue;

		float dx = other._Pos.x - _Pos.x;
		float dy = other._Pos.y - _Pos.y;
		float r = auraRadius + other._radius;

		if (dx * dx + dy * dy < r * r)
		{
			other.speedMultiplier = std::max(other.speedMultiplier, auraSpeedBoost);
		}
	}
}

void AI_Agent::HealthAura(std::vector<AI_Agent>& Enemies)
{
	for (auto& other : Enemies)
	{
		if (!other.active || &other == this) continue;

		float dx = other._Pos.x - _Pos.x;
		float dy = other._Pos.y - _Pos.y;
		float r = auraRadius + other._radius;

		if (dx * dx + dy * dy < r * r)
		{
			other.healthMultiplier = std::max(other.healthMultiplier, auraHealthBoost);
		}
	}
}

void AI_Agent::SlowAura(Player& _player)
{
	float dx = _player._pos.x - _Pos.x;
	float dy = _player._pos.y - _Pos.y;
	float r = auraRadius + _player._radius;

	if (dx * dx + dy * dy < r * r)
	{
		_player.SpeedMultiplier = std::min(_player.SpeedMultiplier, 0.5f);
	}
}

void AI_Agent::CollisionWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius)
{
	float dx = otherPos.x - selfPos.x;
	float dy = otherPos.y - selfPos.y;

	float distance = sqrtf(dx * dx + dy * dy);
	float overlap = (selfRadius + otherRadius) - distance;

	if (overlap <= 0.0f)
		return;

	float nx, ny;
	if (distance > 0.0001f)
	{
		nx = dx / distance;
		ny = dy / distance;
	}
	else
	{
		nx = 1.0f;
		ny = 0.0f;
	}

	float push = overlap * 0.5f;
	selfPos.x -= nx * push;
	selfPos.y -= ny * push;
	otherPos.x += nx * push;
	otherPos.y += ny * push;
}

void AI_Agent::Draw(Texture texture, int screenWidth, int screenHeight)
{
	Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
	
	float destWidth = _radius * 3.0f;
	float destHeight = _radius * 3.0f;

	Rectangle destRec =
	{
		_Pos.x,
		_Pos.y,
		destWidth,
		destHeight
	};

	Vector2 origin = { destWidth / 2.0f, destHeight / 2.0f };

	DrawTexturePro(texture, src, destRec, origin, 0.0f, WHITE);
}

void AI_Agent::drawAura()
{
	Color c = (type == EnemyType::MiniBossH) ? GREEN : YELLOW;
	DrawCircleV(_Pos, auraRadius, Fade(c, 0.1f));
	DrawCircleLinesV(_Pos, auraRadius, c);
}

void AI_Agent::DrawSquare()
{
	float side = _radius * 2.0f;

	DrawRectangleV(
		{ _Pos.x - _radius, _Pos.y - _radius },
		{ side, side }, _colour
	);
}

void AI_Agent::setHealth(float dmg)
{
	Health = Health - (dmg / healthMultiplier);
}

void AI_Agent::DrawTriangle()
{
	Vector2 a = { _Pos.x, _Pos.y - _radius };
	Vector2 b = { _Pos.x - _radius, _Pos.y + _radius };
	Vector2 c = { _Pos.x + _radius, _Pos.y + _radius };
	::DrawTriangle(a, b, c, _colour);
}