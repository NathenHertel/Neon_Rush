#pragma once
#include "raylib.h"
#include "raymath.h"

class Player
{
public:
	Vector2 _pos{ 0.0f, 0.0f };
	int health = 9;
	float _radius = 20.0f;

	void setHealth(int damage);
};