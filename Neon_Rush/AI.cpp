#include "AI.h"

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

void AI_Agent::Draw()
{
	DrawCircleV(_Pos, _radius, _colour);
}