#pragma once
#include "raylib.h"

struct Assets
{
	Texture player{};
	Texture enemy{};
	Sound enemyVoice{};

	void Load();
	void Unload();

private:
	bool loaded = false;
};