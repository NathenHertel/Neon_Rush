#pragma once

#include "raylib.h"
#include "Shop.h"
#include "Config.h"
#include "Player.h"
#include "SceneManager.h"

class ShopScene : public Scene
{
	Player& player;
public:
	ShopScene(Player& p) : player(p) {}

	bool IsOverlay() const override { return true; }

	void Update(float) override
	{
		if (IsKeyPressed(KEY_TAB)) { scenes->Pop(); return; }
	}

	void Render() override
	{
		DrawShop(&player, GetMousePosition(), screenWidth, screenHeight);
	}
};