import XEngine.Engine;
import XEngine.Utils.Logger;

int main() 
{
    Engine e(1020, 800, "XEngine - Best Engine");
    
    if (!e.Initialize()) 
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize game");
        return -1;
    }
    
    e.Run();
    e.Shutdown();
    
    return 0;
}