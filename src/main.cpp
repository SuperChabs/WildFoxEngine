#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <filesystem>

#include "core/shader.hpp"
#include "core/camera.hpp"
#include "scene/model.hpp"

//#define STBI_NO_FAILURE_STRINGS
#include <stb_image.h>

using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(std::vector<std::string> faces);
void renderCube(unsigned int VAO);
void renderQuad();
void renderPlane(unsigned int VAO);
void renderSkybox(unsigned VAO, unsigned int texture);
void renderScene(Shader& shader, unsigned int cubeVAO, unsigned int planeVAO);

float mixValue = 0.2f;

const int width = 800;
const int height = 600;

Camera camera = vec3(0.0f, 0.0f, 3.0f);
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: инициализация и конфигурирование
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    else
        std::cout << "Successful created GLFW window\n";

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    else
        std::cout << "Successful initialize GLAD\n";

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    Shader shader("assets/shaders/vertex/shadow_mapping.vs", "assets/shaders/fragment/shadow_mapping.fs"); 
    // Shader lampShader("assets/shaders/vertex/lamp.vs", "assets/shaders/fragment/lamp.fs");
    Shader skyboxShader("assets/shaders/vertex/skybox.vs", "assets/shaders/fragment/skybox.fs");
    Shader simpleDepthShader("assets/shaders/vertex/shadow_mapping_depth.vs", "assets/shaders/fragment/shadow_mapping_depth.fs");
    Shader debugDepthQuad("assets/shaders/vertex/debug_quad.vs", "assets/shaders/fragment/debug_quad.fs");

    float vCubeVertexes[] = 
    {
        // Задняя грань
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // Передняя грань
        -0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,

        // Грань слева
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f,

        // Грань справа
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        // Нижняя грань 
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,

        // Верхняя грань
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    float vCubeNormales[] = 
    {
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
    
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
    
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f, 
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
    
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
    
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
    
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    float vCubeTextures[] = 
    {
        // Задняя грань
        0.0f, 0.0f, // нижняя-левая
        1.0f, 0.0f, // нижняя-правая    
        1.0f, 1.0f, // верхняя-правая              
        1.0f, 1.0f, // верхняя-правая
        0.0f, 1.0f, // верхняя-левая
        0.0f, 0.0f, // нижняя-левая   
                
        // Передняя грань
        0.0f, 0.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-правая        
        1.0f, 1.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-левая
        0.0f, 1.0f, // верхняя-левая  
        
        // Грань слева
        1.0f, 0.0f, // верхняя-правая
        0.0f, 1.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-левая       
        0.0f, 1.0f, // нижняя-левая
        1.0f, 0.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-правая
    
        // Грань справа
        1.0f, 0.0f, // верхняя-левая
        1.0f, 1.0f, // верхняя-правая      
        0.0f, 1.0f, // нижняя-правая          
        0.0f, 1.0f, // нижняя-правая
        0.0f, 0.0f, // нижняя-левая
        1.0f, 0.0f, // верхняя-левая
    
        // Нижняя грань          
        0.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-левая        
        1.0f, 0.0f, // нижняя-левая
        0.0f, 1.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-правая
    
        // Верхняя грань
        0.0f, 1.0f, // верхняя-левая
        1.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-правая                 
        1.0f, 0.0f, // нижняя-правая
        0.0f, 0.0f, // нижняя-левая  
        0.0f, 1.0f  // верхняя-левая  
    };

    float planeVertices[] = 
    {
      // координаты          // нормали         // текстурные координати 
         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  20.0f, 0.0f,
        -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  0.0f,  20.0f,

         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  20.0f, 0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  0.0f,  20.0f,
         50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  20.0f, 20.0f
    };

    float skyboxVertices[] = 
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes) + sizeof(vCubeNormales) + sizeof(vCubeTextures), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vCubeVertexes), &vCubeVertexes); //вершины
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes), sizeof(vCubeNormales), &vCubeNormales); //нормали
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes) + sizeof(vCubeNormales), sizeof(vCubeTextures), &vCubeTextures); // текстуры

    // VAO куба
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)sizeof(vCubeVertexes));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(vCubeVertexes) + sizeof(vCubeNormales)));
    glBindVertexArray(0);
	
    // VAO пола
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

	// VAO скайбокса
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Настраиваем карту глубины FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
	
    // Создаем текстуры глубины
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
    // Прикрепляем текстуру глубины в качестве буфера глубины для FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Загрузка текстур
    unsigned int cubeTexture  = loadTexture("assets/textures/wooden_container.png");
    unsigned int floorTexture = loadTexture("assets/textures/wood.png");
    unsigned int errorTexture = loadTexture("assets/textures/error.png");

    std::vector<std::string> faces = 
    {
        "assets/textures/skybox2/right.png",
        "assets/textures/skybox2/left.png",
        "assets/textures/skybox2/top.png",
        "assets/textures/skybox2/bottom.png",
        "assets/textures/skybox2/front.png",
        "assets/textures/skybox2/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    // Конфигурация шейдеров
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);

    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0); 

    vec3 lightPos = vec3(3.0f, 2.0f, 1.0f);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) 
    {
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebuffer_size_callback(window, fbWidth, fbHeight);

        // Обработка ввода
        processInput(window); 

        // glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);        

        // Рендеринг
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. Рендеринг глубины сцены в текстуру (вид - с позиции источника света)
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
		
        // Рендеринг сцены глазами источника света
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            renderScene(simpleDepthShader, cubeVAO, planeVAO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Сброс настроек области просмотра
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени 
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
		
        // Устанавливаем uniform-переменные освещения
        shader.setVec3("viewPos", camera.GetPosition());
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(shader, cubeVAO, planeVAO);

        // Рендеринг на плоскости карты глубины для наглядной отладки
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //renderQuad();

        // SkyBox
        skyboxShader.use();
        mat4 skyboxView = mat4(mat3(view));
        skyboxShader.setMat4("view", skyboxView);
        skyboxShader.setMat4("projection", projection);
        renderSkybox(skyboxVAO, cubemapTexture); 

        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "End \n";
    std::cout << glGetString(GL_VERSION) << std::endl;

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &planeVBO);

    // glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
    glfwTerminate();

    return 0;
}

void renderScene(Shader& shader, unsigned int cubeVAO, unsigned int planeVAO) 
{
    mat4 model = mat4(1.0f);
    shader.setMat4("model", model);
    renderPlane(planeVAO);
	
    // Ящики
    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, 1.5f, 0.0));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO);
    model = mat4(1.0f);
    model = translate(model, vec3(2.0f, 0.0f, 1.0));
    model = scale(model, vec3(0.5f));
    shader.setMat4("model", model);
    renderCube(cubeVAO);
    model = mat4(1.0f);
    model = translate(model, vec3(-1.0f, 0.0f, 2.0));
    model = rotate(model, radians(60.0f), normalize(vec3(1.0, 0.0, 1.0)));
    model = scale(model, vec3(0.25));
    shader.setMat4("model", model);
    renderCube(cubeVAO);
}

void renderCube(unsigned int VAO) 
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
             // координаты      // текстурные координаты
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
		
        // Установка VAO пола
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void renderPlane(unsigned int VAO) 
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderSkybox(unsigned VAO, unsigned int texture) 
{
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    glEnable(GL_DEPTH_TEST);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse) 
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
    camera.ProcessScrool(yoffset);
}

void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam)
{
    std::cerr << "GL DEBUG [" << id << "]: " << message << std::endl; //
    std::cerr << "Type: " << type << ", Severity: " << severity << ", Source: " << source << std::endl;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        GLenum tex;
        if (nrComponents == 1)
        {    
            format = GL_RED;
            tex = GL_REPEAT;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
            tex = GL_REPEAT;
        }   
        else if (nrComponents == 4) 
        {
            format = GL_RGBA;
            tex = GL_CLAMP_TO_EDGE;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) 
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        } 
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
