#pragma once

#include "raylib.h"
#include "Player.h"

void ApplyToPlayer(Player* player, int chosenNum);
void DrawShop(Player* player, Vector2 mousePos, int screenWidth, int screenHeight);