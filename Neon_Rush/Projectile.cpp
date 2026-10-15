#include "Projectile.h"

Bullet::Bullet()
{

}

Bullet::Bullet(Vector2 startPos, float radius) :
	_pos(startPos),
	_radius(radius)
{

}

void Bullet::Update(float deltaTime, int screenWidth, int screenHeight)
{
	if (_disabled) return;

	_pos.x += _vel.x * deltaTime;
	_pos.y += _vel.y * deltaTime;

	if (_pos.x < 0 || _pos.x > screenWidth || _pos.y < 0 || _pos.y > screenHeight)
		_disabled = true;
}

void Bullet::Draw()
{
	if (_disabled) return;
	DrawCircleV(_pos, _radius, _colour);
}