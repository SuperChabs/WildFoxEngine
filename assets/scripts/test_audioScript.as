string soundPath = "../assets/sounds/meow.wav";
float volume = 1.0f;
float pitch = 1.0f;

void OnTriggerEnter(uint64 otherEntity)
{
    PlayOneShot(soundPath, volume, pitch);
}
