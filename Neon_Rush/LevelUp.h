#pragma once
#include "Player.h"
#include "Agent.h"

void LvlUp(Player* player, float _exp);
void DrawLevelUpMenu(Player* player, Vector2 mousePos, int screenWidth, int screenHeight, Font font);