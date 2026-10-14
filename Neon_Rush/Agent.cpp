#include "Agent.h"

AI_Agent::AI_Agent()
{
	_Pos = Vector2{ 0, 0 };
}

AI_Agent::AI_Agent(Vector2 pos, float radius, Color colour) :
	_Pos{ pos },
	_radius{ radius },
	_colour{ colour }
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

void AI_Agent::Draw()
{
	DrawCircleV(_Pos, _radius, _colour);
}