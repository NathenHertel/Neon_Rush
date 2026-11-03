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

void AI_Agent::Update(float deltaTime, Vector2 playerPos, std::vector<AI_Agent>& Enemies, std::vector<Bullet>& EnemyBullets)
{
	for (auto& AI_Agent : Enemies)
	{
		if (!AI_Agent.active) continue;
		if (AI_Agent.ranged)
		{
			AI_Agent.ShootAtPlayer(playerPos, deltaTime, EnemyBullets);
		}
		else
		{
			AI_Agent.MoveToPlayer(playerPos, 350, deltaTime);
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

	EnemyBullets.push_back(newBullet);
}

bool AI_Agent::CheckOverlapWithOtherAgent(Vector2& selfPos, Vector2& otherPos, float selfRadius, float otherRadius)
{
	float dx = otherPos.x - selfPos.x;
	float dy = otherPos.y - selfPos.y;
	float radiusSum = selfRadius + otherRadius;

	return (dx * dx + dy * dy) < (radiusSum * radiusSum);
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
	//DrawCircleV(_Pos, _radius, _colour);
}

void AI_Agent::setHealth(int dmg)
{
	Health = Health - dmg;
}