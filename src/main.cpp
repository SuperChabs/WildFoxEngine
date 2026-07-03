#include "engine/Engine.h"
#include "core/logging/Logger.h"

int main() {
    Engine e(1020, 800, "Allusion");

    if (!e.Initialize()) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize game");
        return -1;
    }

    e.Run();
    e.Shutdown();

    return 0;
}