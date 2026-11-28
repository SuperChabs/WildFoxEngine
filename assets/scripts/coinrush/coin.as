#include "../core/base.as"

// Coin Rush coin: bobs in place and gets collected when the player touches it.
// A collected coin is moved below the arena (y = -1000); game_manager counts those.
// `self` and _SetEntity come from base.as, don't redeclare them here.

class Coin
{
    float  bobHeight;
    float  bobSpeed;

    uint64 entity;
    uint64 player;
    vec3   basePos;
    float  time;
    bool   collected;

    Coin()
    {
        bobHeight = 0.15f;
        bobSpeed  = 3.0f;
        time      = 0.0f;
        collected = false;
    }

    void Start(uint64 owner)
    {
        entity = owner;
        player = GetEntityByName("player");

        Transform@ t = GetTransform(entity);
        if (t is null) return;

        basePos = t.position;
        // different phase per coin, so they don't bob in sync
        time = basePos.x + basePos.z;
    }

    void Update(float dt)
    {
        if (collected) return;

        Transform@ t = GetTransform(entity);
        if (t is null) return;

        time += dt;
        t.position.y = basePos.y + sin(time * bobSpeed) * bobHeight;
    }

    void TriggerEnter(uint64 other)
    {
        if (collected || other != player) return;

        Transform@ t = GetTransform(entity);
        if (t is null) return;

        collected = true;
        t.position = vec3(basePos.x, -1000.0f, basePos.z);
    }
}

Coin coin;

void OnStart()
{
    coin.Start(self);
}

void OnUpdate(float dt)
{
    coin.Update(dt);
}

void OnTriggerEnter(uint64 other)
{
    coin.TriggerEnter(other);
}
