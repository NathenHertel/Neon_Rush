#include "Shop.h"

int chosenNum = 0;

int itemCost[6] = { 250, 250, 350, 500, 500, 1000 };

void ApplyToPlayer(Player* player, int chosenNum)
{
	if (!player) return;

    switch (chosenNum)
    {
    case 0: // Rapid Fire
        if (player->points >= itemCost[chosenNum])
        {
            player->rapidFireActive = true;
            player->rapidFireTimer = player->rapidFireDur;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;
        }
        break;

    case 1: // Triple Shot
        if (player->points >= itemCost[chosenNum])
        {
            player->tripleShotActive = true;
            player->tripleShotTimer = player->tripleShotDur;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;
        }
        break;

    case 2: // Shield 
        if (player->points >= itemCost[chosenNum])
        {
            player->shieldAmt += 1;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;
        }
        break;

    case 3: // Damage Boost
        if (player->points >= itemCost[chosenNum])
        {
            player->damage *= 1.1f;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;    
        }
        break;
    case 4: // Speed Boost
        if (player->points >= itemCost[chosenNum])
        {
            player->playerSpeed *= 1.05f;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;
        }
        break;
    case 5: // Extra Life
        if (player->points >= itemCost[chosenNum])
        {
            player->hasExtraLife = true;
            player->points -= itemCost[chosenNum];
            itemCost[chosenNum] *= 2;
        }
        else
        {
            std::cout << "You dont have enough points" << std::endl;
        }
        break;
	}

}

void DrawShop(Player* player, Vector2 mousePos, int screenWidth, int screenHeight)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

    const int   COLS = 3;
    const int   COUNT = 6;
    const float CARD_W = 180.0f;
    const float CARD_H = 200.0f;
    const float GAP = 20.0f;

    int   rows = (COUNT + COLS - 1) / COLS;                 // 2
    float gridW = COLS * CARD_W + (COLS - 1) * GAP;
    float gridH = rows * CARD_H + (rows - 1) * GAP;
    float startX = (screenWidth - gridW) / 2.0f;
    float startY = (screenHeight - gridH) / 2.0f;

    const char* titles[COUNT] = { "Rapid Fire", "Triple Shot", "Shield", "Damage", "Speed", "Extra Life" };

    const char* costLabel = TextFormat("%d pts", itemCost[chosenNum]);

    for (int i = 0; i < COUNT; i++)
    {
        int col = i % COLS;       // 0,1,2,0,1,2
        int row = i / COLS;       // 0,0,0,1,1,1

        float x = startX + col * (CARD_W + GAP);
        float y = startY + row * (CARD_H + GAP);

        Rectangle card = { x,       y,                  CARD_W,       CARD_H };
        Rectangle title = { x + 10,  y + 8,              CARD_W - 20,  30 };
        Rectangle img = { x + 20,  y + 45,             CARD_W - 40,  80 };
        Rectangle button = { x + 20,  y + CARD_H - 50,    CARD_W - 40,  36 };

        DrawRectangleRec(card, DARKGRAY);

        int tw = MeasureText(titles[i], 20);
        DrawText(titles[i],
            (int)(title.x + (title.width - tw) / 2),
            (int)(title.y + (title.height - 20) / 2),
            20, WHITE);

        DrawRectangleRec(img, GRAY);

        bool hover = CheckCollisionPointRec(mousePos, button);
        DrawRectangleRec(button, hover ? RED : MAROON);

        const char* costLabel = TextFormat("%d pts", itemCost[i]);
        int bw = MeasureText(costLabel, 18);
        DrawText(costLabel,
            (int)(button.x + (button.width - bw) / 2),
            (int)(button.y + (button.height - 18) / 2),
            18, WHITE);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            ApplyToPlayer(player, i);
    }
}