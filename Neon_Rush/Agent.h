#pragma once

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

class AI_Agent
{
public:
	float maxSpeed{ 50.0f };;
	Vector2 Accel;
	Vector2 Vel;

	AI_Agent();
	AI_Agent(Vector2 pos, float radius, Color colour);

	virtual ~AI_Agent() {};

	virtual void Update(float deltaTime);
	virtual void Draw();

	void SetPos(Vector2 pos);
	Vector2 GetPos();

	void MoveToPlayer(Vector2& targetPos, float speed, float deltaTime);

private:
	Vector2 _Pos{ 0.0f, 0.0f };
	float _radius{ 5.0f };
	Color _colour{ BLUE };
};