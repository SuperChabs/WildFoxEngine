import WFE.Engine;
import WFE.Core.Logger;

int main() 
{
#if defined (__WIN32__)
    exit()
#endif

    Engine e(1020, 800, "Allusion");
    
    if (!e.Initialize()) 
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize game");
        return -1;
    }
    
    e.Run();
    e.Shutdown();
    
    return 0;
}