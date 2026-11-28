#pragma once

#include "raylib.h"
#include "raymath.h"
#include "Config.h"
#include "Balance.h"
#include "Player.h"

// Bottom-left HUD: [wave icon] [HP bar / shield bar]
namespace Hud
{
    inline void DrawBar(const Font& font, Rectangle r, float frac, Color fill, const char* label, float fontSize)
    {
        frac = Clamp(frac, 0.0f, 1.0f);

        DrawRectangleRec(r, Fade(BLACK, 0.75f));
        DrawRectangleRec({ r.x, r.y, r.width * frac, r.height }, fill);
        DrawRectangleLinesEx(r, 2.0f, Fade(WHITE, 0.8f));

        Vector2 ts = MeasureTextEx(font, label, fontSize, 1.0f);
        DrawTextEx(font, label,
            { r.x + (r.width - ts.x) / 2.0f, r.y + (r.height - ts.y) / 2.0f },
            fontSize, 1.0f, WHITE);
    }

    // cdFrac: 0 = ready, 1 = just used.
    inline void DrawWaveIcon(const Font& font, Vector2 c, float r, float cdFrac, float secsLeft)
    {
        const bool  ready = cdFrac <= 0.0f;
        const Color main = ready ? Color{ 90, 220, 255, 255 } : Color{ 120, 120, 140, 255 };

        DrawCircleV(c, r, Fade(BLACK, 0.75f));

        // Expanding-ring symbol
        DrawRing(c, r * 0.18f, r * 0.30f, 0.0f, 360.0f, 32, main);
        DrawRing(c, r * 0.45f, r * 0.55f, 0.0f, 360.0f, 32, main);
        DrawRing(c, r * 0.72f, r * 0.80f, 0.0f, 360.0f, 32, main);

        if (!ready)
        {
            // Dark pie covering the part of the cooldown that is still left, sweeping away clockwise.
            DrawCircleSector(c, r, -90.0f + 360.0f * (1.0f - cdFrac), 270.0f, 48, Fade(BLACK, 0.7f));

            const char* t = TextFormat("%d", (int)ceilf(secsLeft));
            Vector2 ts = MeasureTextEx(font, t, 22, 1.0f);
            DrawTextEx(font, t, { c.x - ts.x / 2.0f, c.y - ts.y / 2.0f }, 22, 1.0f, WHITE);
        }

        DrawCircleLinesV(c, r, ready ? WHITE : GRAY);

        // Key hint under the icon
        Vector2 ks = MeasureTextEx(font, "Q", 16, 1.0f);
        DrawTextEx(font, "Q", { c.x - ks.x / 2.0f, c.y + r + 2.0f }, 16, 1.0f, ready ? WHITE : GRAY);
    }

    inline void Draw(const Player& p, const Font& font)
    {
        const float margin = 24.0f;
        const float iconR = 28.0f;
        const Vector2 iconC = { margin + iconR, screenHeight - margin - iconR - 12.0f };

        DrawWaveIcon(font, iconC,
            iconR,
            p.waveCooldownTimer / Balance::WAVE_CLEAR_COOLDOWN,
            p.waveCooldownTimer);

        const float barX = iconC.x + iconR + 16.0f;
        const float barW = 280.0f;

        // ----- Health -----
        Rectangle hpBar = { barX, iconC.y - 22.0f, barW, 26.0f };
        DrawBar(font, hpBar, p.health / p.maxHealth, Color{ 210, 45, 55, 255 },
            TextFormat("%d / %d", (int)ceilf(p.health), (int)ceilf(p.maxHealth)), 18.0f);

        if (p.hasExtraLife)
            DrawTextEx(font, "+1 LIFE", { hpBar.x + hpBar.width + 8.0f, hpBar.y + 4.0f }, 18, 1.0f, GOLD);

        // ----- Shield -----
        Rectangle shBar = { barX, hpBar.y + hpBar.height + 6.0f, barW, 18.0f };

        if (p.shieldCooldownTimer > 0.0f)
        {
            const float frac = 1.0f - p.shieldCooldownTimer / Balance::SHIELD_COOLDOWN;
            DrawBar(font, shBar, frac, Color{ 90, 100, 130, 255 },
                TextFormat("Shield recharging %ds", (int)ceilf(p.shieldCooldownTimer)), 14.0f);
        }
        else
        {
            DrawBar(font, shBar, p.shield / p.shieldMax, SKYBLUE,
                TextFormat("Shield %d / %d", (int)ceilf(p.shield), (int)ceilf(p.shieldMax)), 14.0f);
        }

        // ----- Level progress (strip along the bottom edge) -----
        Rectangle xp = { 0.0f, screenHeight - 14.0f, (float)screenWidth, 14.0f };
        DrawRectangleRec(xp, Fade(BLACK, 0.75f));
        DrawRectangleRec({ xp.x, xp.y, xp.width * Clamp(p._exp / p.requiredExp, 0.0f, 1.0f), xp.height },
            Color{ 255, 200, 40, 255 });

        const char* xpText = TextFormat("Level %d   %d / %d XP", p.level, (int)p._exp, (int)p.requiredExp);
        Vector2 xs = MeasureTextEx(font, xpText, 12, 1.0f);
        DrawTextEx(font, xpText, { (screenWidth - xs.x) / 2.0f, xp.y + (xp.height - xs.y) / 2.0f }, 12, 1.0f, WHITE);
    }
}
