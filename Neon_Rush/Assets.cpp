#include "Assets.h"

void Assets::Load()
{
	player = LoadTexture("Textures/IMG_1138.png");
	enemy = LoadTexture("Textures/IMG_1139.png");
	enemyVoice = LoadSound("SoundEffects/spongebob-hi-how-are-ya-.mp3");
	font = LoadFontEx("Fonts/Roboto-Black.ttf", 64, nullptr, 0);

	GenTextureMipmaps(&player);
	SetTextureFilter(player, TEXTURE_FILTER_TRILINEAR);

	GenTextureMipmaps(&enemy);
	SetTextureFilter(enemy, TEXTURE_FILTER_TRILINEAR);

	GenTextureMipmaps(&font.texture);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	LaserShot = LoadSound("SoundEffects/LaserSound1.mp3");
	SetSoundVolume(LaserShot, 0.5f);

	EnemyDeath = LoadSound("SoundEffects/DeathSound.mp3");
	SetSoundVolume(EnemyDeath, 0.5f);

	if (player.id == 0) TraceLog(LOG_ERROR, "Player Texture Failed to load");
	if (enemy.id == 0) TraceLog(LOG_ERROR, "Enemy Texture Failed to load");
	if (enemyVoice.frameCount == 0) TraceLog(LOG_ERROR, "Enemy Sound failed to load");
	if (LaserShot.frameCount == 0) TraceLog(LOG_ERROR, "Laser Sound failed to load");
	if (EnemyDeath.frameCount == 0) TraceLog(LOG_ERROR, "EnemyDeath Sound failed to load");
	if (!IsFontValid(font) || font.texture.id == GetFontDefault().texture.id)
	{
		std::cout << "FONT FAILED TO LOAD, using default" << std::endl;
	}
	else
	{
		std::cout << "Font loaded, glyphs: " << font.glyphCount << std::endl;
	}

	loaded = true;
}

void Assets::Unload()
{
	if (!loaded) return;

	UnloadTexture(player);
	UnloadTexture(enemy);
	UnloadSound(enemyVoice);
	UnloadSound(LaserShot);
	UnloadSound(EnemyDeath);
	UnloadFont(font);

	loaded = false;
}