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

void AI_Agent::Update(float deltaTime)
{

}

void AI_Agent::MoveToPlayer(Vector2& targetPos, float speed, float deltaTime)
{
	if (Vector2Distance(_Pos, targetPos) > 2.0f)
	{
		Vector2 Direction = Vector2Normalize(Vector2Subtract(targetPos, _Pos));
		_Pos = Vector2Add(_Pos, Vector2Scale(Direction, speed * deltaTime));
	}
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

void AI_Agent::Draw()
{
	DrawCircleV(_Pos, _radius, _colour);
}