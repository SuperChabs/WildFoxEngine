uint64 self;

void _SetEntity(uint64 e)
{
    self = e;
}

void OnStart()
{
}

void OnTriggerEnter(uint64 other)
{
    Transform@ t = GetTransform(self);
    if (t is null) return;
    t.position.y += 5.0f;
}

void OnTriggerExit(uint64 other)
{
    Transform@ t = GetTransform(self);
    if (t is null) return;
    t.position.y -= 5.0f;
}