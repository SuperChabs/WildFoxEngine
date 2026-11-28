#include "../core/base.as"

// Coin Rush game manager: counts collected coins (coin_1 .. coin_N) and announces the win.
// There are no script arrays yet, so coins are looked up by name.

int coinCount = 6;

int   lastCollected = -1;
float elapsed       = 0.0f;
bool  won           = false;

void OnStart()
{
    Log("Coin Rush: collect all " + coinCount + " coins! WASD to move, SHIFT to sprint");
}

void OnUpdate(float dt)
{
    if (won) return;

    elapsed += dt;

    int collected = 0;
    for (int i = 1; i <= coinCount; i++)
    {
        Transform@ t = GetTransform(GetEntityByName("coin_" + i));
        if (t !is null && t.position.y < -500.0f)
            collected++;
    }

    if (collected == lastCollected) return;
    lastCollected = collected;

    if (collected > 0)
        Log("Coins: " + collected + "/" + coinCount);

    if (collected == coinCount)
    {
        won = true;
        Log("YOU WIN! Time: " + elapsed + " s");
    }
}
