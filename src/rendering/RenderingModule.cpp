#include "RenderingModule.h"
#include <string>
#include <vector>
#include "core/logging/Logger.h"

RenderingModule::RenderingModule(GLFWwindow *win, ECSWorld *ecs, ModuleManager *mm)
    : window(win)
      , ecsWorld(ecs) {
    if (!window)
        Logger::Log(LogLevel::ERROR, "RenderingModule: window is null!");
    if (!ecsWorld)
        Logger::Log(LogLevel::ERROR, "RenderingModule: ecsWorld is null!");

    resourceModule = mm->GetModule<ResourceModule>("Resource");
}

bool RenderingModule::Initialize() {
    Logger::Log(LogLevel::INFO, "=== Initializing RenderingModule ===");

    if (!InitializeOpenGL()) {
        Logger::Log(LogLevel::WARNING,
                    "OpenGL not available - running in headless mode");
        hasOpenGL = false;
        return true;
    }

    hasOpenGL = true;

    if (!LoadResources()) {
        Logger::Log(LogLevel::ERROR, "Failed to load resources");
        return false;
    }

    if (!CreateSkybox()) {
        Logger::Log(LogLevel::WARNING, "Failed to create skybox");
    }

    if (!CreateRenderer()) {
        Logger::Log(LogLevel::ERROR, "Failed to create renderer");
        return false;
    }

    if (!InitializeRenderer()) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize renderer");
        return false;
    }

    isInitialized = true;
    Logger::Log(LogLevel::INFO, "RenderingModule initialized successfully");
    Logger::Log(LogLevel::INFO, "======================================");

    return true;
}

void RenderingModule::Update(float deltaTime) {
}

void RenderingModule::Shutdown() {
    Logger::Log(LogLevel::INFO, "Shutting down RenderingModule...");

    if (renderer) {
        renderer->Shutdown();
        renderer.reset();
    }

    skybox.reset();

    isInitialized = false;
    Logger::Log(LogLevel::INFO, "RenderingModule shutdown complete");
}

const char *RenderingModule::GetName() const {
    return "Rendering";
}

int RenderingModule::GetPriority() const {
    return 30;
}

bool RenderingModule::IsRequired() const {
    return false;
}

Renderer *RenderingModule::GetRenderer() {
    return renderer.get();
}

Skybox *RenderingModule::GetSkybox() {
    return skybox.get();
}

bool RenderingModule::HasOpenGL() const {
    return hasOpenGL;
}

bool RenderingModule::InitializeOpenGL() {
    Logger::Log(LogLevel::INFO, "Checking OpenGL availability...");

    if (!window) {
        Logger::Log(LogLevel::ERROR, "No window available for OpenGL context");
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Logger::Log(LogLevel::WARNING, "Failed to load OpenGL functions");
        return false;
    }

    const char *vendor = (const char *) glGetString(GL_VENDOR);
    const char *rendererName = (const char *) glGetString(GL_RENDERER);
    const char *version = (const char *) glGetString(GL_VERSION);

    Logger::Log(LogLevel::INFO, "OpenGL Vendor: " + std::string(vendor ? vendor : "Unknown"));
    Logger::Log(LogLevel::INFO, "OpenGL Renderer: " + std::string(rendererName ? rendererName : "Unknown"));
    Logger::Log(LogLevel::INFO, "OpenGL Version: " + std::string(version ? version : "Unknown"));

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    return true;
}

bool RenderingModule::LoadResources() {
    Logger::Log(LogLevel::INFO, "Loading resources...");

    try {
        resourceModule->GetShaderManager()->Load();
        Logger::Log(LogLevel::INFO, "Shaders loaded successfully");

        resourceModule->GetMaterialManager()->LoadMaterialConfigs();
        Logger::Log(LogLevel::INFO, "Materials loaded successfully");

        resourceModule->GetShaderManager()->Bind("skybox");
        resourceModule->GetShaderManager()->SetInt("skybox", "skybox", 0);
        resourceModule->GetShaderManager()->Unbind();

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception loading resources: " + std::string(e.what()));
        return false;
    }
}

bool RenderingModule::CreateSkybox() {
    Logger::Log(LogLevel::INFO, "Creating skybox...");

    try {
        std::vector<std::string> faces =
        {
            "assets/textures/skybox4/right.bmp",
            "assets/textures/skybox4/left.bmp",
            "assets/textures/skybox4/top.bmp",
            "assets/textures/skybox4/bottom.bmp",
            "assets/textures/skybox4/front.bmp",
            "assets/textures/skybox4/back.bmp"
        };

        //     std::vector<std::string> faces = 
        //    {
        //         "assets/textures/error.png",
        //         "assets/textures/error.png",
        //         "assets/textures/error.png",
        //         "assets/textures/error.png",
        //         "assets/textures/error.png",
        //         "assets/textures/error.png"
        //     }; 

        unsigned int cubemapTexture = resourceModule->GetTextureManager()->LoadCubemap(faces);

        if (cubemapTexture == 0) {
            Logger::Log(LogLevel::ERROR, "Failed to load cubemap texture");
            return false;
        }

        skybox = std::make_unique<Skybox>(cubemapTexture, "skybox");

        Logger::Log(LogLevel::INFO, "Skybox created successfully");
        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating skybox: " + std::string(e.what()));
        return false;
    }
}

bool RenderingModule::CreateRenderer() {
    Logger::Log(LogLevel::INFO, "Creating renderer...");

    if (!ecsWorld) {
        Logger::Log(LogLevel::ERROR, "ECSWorld is null, cannot create renderer");
        return false;
    }

    try {
        renderer = std::make_unique<Renderer>(resourceModule->GetShaderManager(), ecsWorld,
                                              resourceModule->GetTextureManager());

        Logger::Log(LogLevel::INFO, "Renderer created successfully");
        Logger::Log(LogLevel::DEBUG, "  - ShaderManager: " +
                                     std::to_string(
                                         reinterpret_cast<uintptr_t>(resourceModule->GetShaderManager())));
        Logger::Log(LogLevel::DEBUG, "  - ECSWorld: " +
                                     std::to_string(reinterpret_cast<uintptr_t>(ecsWorld)));

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating renderer: " + std::string(e.what()));
        return false;
    }
}

bool RenderingModule::InitializeRenderer() {
    Logger::Log(LogLevel::INFO, "Initializing renderer...");

    if (!renderer) {
        Logger::Log(LogLevel::ERROR, "Renderer is null");
        return false;
    }

    if (!skybox) {
        Logger::Log(LogLevel::WARNING, "Skybox is null, using fallback");
    }

    GLuint skyboxVAO = skybox ? skybox->GetVAO() : 0;
    GLuint skyboxTexture = skybox ? skybox->GetTexture() : 0;

    if (!renderer->Initialize(skyboxVAO, skyboxTexture)) {
        Logger::Log(LogLevel::ERROR, "Renderer initialization failed");
        return false;
    }

    Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
    return true;
}