#pragma once

#include "raylib.h"
#include "Shop.h"
#include "Config.h"
#include "Player.h"
#include "GamepadInput.h"
#include "SceneManager.h"

class ShopScene : public Scene
{
	Player& player;
	int selected = 0;
public:
	ShopScene(Player& p) : player(p) {}

	bool IsOverlay() const override { return true; }

	void Update(float) override
	{
		if (IsKeyPressed(KEY_TAB) || Pad::Back() || Pad::B()) { scenes->Pop(); return; }
		
		if (Pad::Available())
		{
			if (Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) selected = (selected + 1) % 6;
			if (Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_LEFT))  selected = (selected + 5) % 6;
			if (Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
				Pad::Pressed(GAMEPAD_BUTTON_LEFT_FACE_UP))    selected = (selected + 3) % 6;   // other row
			if (Pad::A()) ApplyToPlayer(&player, selected);
		}
	}

	void Render() override
	{
		DrawShop(&player, GetMousePosition(), screenWidth, screenHeight, Pad::Available() ? selected : -1);
	}
};