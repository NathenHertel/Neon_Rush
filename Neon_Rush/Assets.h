#pragma once
#include "raylib.h"
#include <iostream>

struct Assets
{
	Texture player{};
	Texture enemy{};
	Sound enemyVoice{};
	Sound LaserShot{};
	Sound EnemyDeath{};
	Sound BackRound{};
	Font font{};

	void Load();
	void Unload();

private:
	bool loaded = false;
};