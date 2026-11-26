#include "engine/Engine.h"
#include "core/logging/Logger.h"

int main() {
    char* title = "WildFox Engine";

    Engine e(1640, 800, title);

    if (!e.Initialize()) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize game");
        return -1;
    }

    e.Run();
    e.Shutdown();

    return 0;
}
