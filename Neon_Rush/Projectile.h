#pragma once
#include "raylib.h"

#include <cmath>


class Bullet
{
public:
	// ----- DECLERATIONS -----
	Vector2 _pos;
	Vector2 _vel;
	bool _disabled = false;
	Color _colour = { RED };
	float _radius;

	Bullet();
	Bullet(Vector2 startPos, float radius);

	virtual ~Bullet() {};

	virtual void Update(float deltaTime, int screenWidth, int screenHeight);
	virtual void Draw();

private:
	int MAX_BULLETS = 500000;
};