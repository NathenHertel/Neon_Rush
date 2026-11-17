#include "Assets.h"

void Assets::Load()
{
	player = LoadTexture("Textures/IMG_1138.png");
	enemy = LoadTexture("Textures/IMG_1139.png");
	enemyVoice = LoadSound("spongebob-hi-how-are-ya-.mp3");

	if (player.id == 0) TraceLog(LOG_ERROR, "Player Texture Failed to load");
	if (enemy.id == 0) TraceLog(LOG_ERROR, "Enemy Texture Failed to load");
	if (enemyVoice.frameCount == 0) TraceLog(LOG_ERROR, "Enemy Sound failed to load");

	loaded = true;
}

void Assets::Unload()
{
	if (!loaded) return;

	UnloadTexture(player);
	UnloadTexture(enemy);
	UnloadSound(enemyVoice);

	loaded = false;
}