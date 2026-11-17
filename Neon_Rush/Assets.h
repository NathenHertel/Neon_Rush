#pragma once
#include "raylib.h"

struct Assets
{
	Texture player{};
	Texture enemy{};
	Sound enemyVoice{};
	Sound laserShot{};
	Sound EnemyDeath{};
	Sound BackRound{};

	void Load();
	void Unload();

private:
	bool loaded = false;
};