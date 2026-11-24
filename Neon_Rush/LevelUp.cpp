#include "LevelUp.h"

void LvlUp(Player* player, float exp)
{
	player->_exp = player->_exp + exp;

	if (player->_exp >= player->requiredExp)
	{
		player->maxHealth = player->maxHealth * 1.10f;
		player->health = player->maxHealth;
		player->_exp = player->_exp - player->requiredExp;
		player->requiredExp = player->requiredExp * 1.5f;
		player->levelUp = true;
	}
}

void DrawLevelUpMenu(Player* player, Vector2 mousePos, int screenWidth, int screenHeight, Font font)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

    const char* titles[3] = { "Health", "Damage", "Attack Speed" };

    for (int i = 0; i < 3; i++)
    {
        float x = 240.0f + i * 280.0f;

        // Create Menu
        Rectangle card = { x,      215.0f, 240.0f, 350.0f };
        Rectangle title = { x + 15, 225.0f, 210.0f, 55.0f };
        Rectangle img = { x + 30, 290.0f, 180.0f, 195.0f };
        Rectangle button = { x + 40, 505.0f, 160.0f, 45.0f };


        DrawRectangleRec(card, DARKGRAY);

        Vector2 ts = MeasureTextEx(font, titles[i], 30, 1.0f);
        DrawTextEx(font, titles[i],
            { title.x + (title.width - ts.x) / 2, title.y + (title.height - ts.y) / 2 },
            30, 1.0f, WHITE);

        DrawRectangleRec(img, GRAY);

        bool hover = CheckCollisionPointRec(mousePos, button);
        DrawRectangleRec(button, hover ? RED : MAROON);

        Vector2 bs = MeasureTextEx(font, "Pick", 24, 1.0f);
        DrawTextEx(font, "Pick",
            { button.x + (button.width - bs.x) / 2, button.y + (button.height - bs.y) / 2 },
            24, 1.0f, WHITE);

        // Find Which Button is pressed for level up option
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            switch (i)
            {
            case 0: // Health
                player->level += 1;
                player->maxHealth = player->maxHealth + 10.0f;
                player->health = player->maxHealth; // Heal On Level Up

                if (player->level % 3 == 0)
                {
                    player->EnemyAmt = player->EnemyAmt + 1;
                }
                

                player->levelUp = false; // Reset status of needed to be leveled up
                break;
            case 1: // Damage
                player->level += 1;
                player->health = player->maxHealth;
                player->damage = player->damage + 5;

                if (player->level % 3 == 0)
                {
                    player->EnemyAmt = player->EnemyAmt + 1;
                }

                player->levelUp = false;
                break;
            case 2: // Attack Speed
                player->level += 1;
                player->health = player->maxHealth;
                player->baseFireInterval = std::max(0.1f, player->baseFireInterval - 0.05f);

                if (player->level % 3 == 0)
                {
                    player->EnemyAmt = player->EnemyAmt + 1;
                }

                player->levelUp = false;
                break;
            }
        }
    }
}