#include "core/Shader.h"
#include "utils/Logger.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    CompileShader(vertexPath, fragmentPath, geometryPath);
}

Shader::Shader(const char *shaderName, bool isGeometry)
{

    if (isGeometry)
    {
        std::string vertexPath = "assets/shaders/vertex/" + std::string(shaderName) + ".vs";
        std::string fragmentPath = "assets/shaders/fragment/" + std::string(shaderName) + ".fs";
        std::string geometryPath = "assets/shaders/geometry/" + std::string(shaderName) + ".gs";

        CompileShader(vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str());
    }    
    else
    {
        std::string vertexPath = "assets/shaders/vertex/" + std::string(shaderName) + ".vs";
        std::string fragmentPath = "assets/shaders/fragment/" + std::string(shaderName) + ".fs";

        CompileShader(vertexPath.c_str(), fragmentPath.c_str());
    }
}

// Активація шейдера
void Shader::use()
{
    glUseProgram(ID);
}

// Корисні uniform-методи
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// Корисні мктоди для перевірки помилок компіляції/зв'язування шейдерів
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") 
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) 
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Logger::Log(LogLevel::ERROR, "SHADER_COMPILATION_ERROR of type: " + type + "\n" + std::string(infoLog) + "\n -- --------------------------------------------------- -- ");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
            Logger::Log(LogLevel::ERROR, "PROGRAM_LINKING_ERROR of type: " + type + "\n" + std::string(infoLog) + "\n -- --------------------------------------------------- -- ");
        }
    }
}

void Shader::CompileShader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
    // Етап №1: Отримання вихідного коду вершинного/фрагментного шейдеру з змінної filePath  
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // Переконуємося, що об'єкти ifstream можуть викинути виключення
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // Відкриваємо файли
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // Зчитуємо вміст файлових буферів в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Зачиняємо файли
        vShaderFile.close();
        fShaderFile.close();

        // Конвертуємо данні з потоку в рядкові змінні
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath != nullptr) 
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure e)
    {
        Logger::Log(LogLevel::ERROR, "SHADER::FILE_NOT_SUCCESFULLY_READ");
        Logger::Log(LogLevel::ERROR, "Cannot open vertex shader at " + std::string(vertexPath));
        Logger::Log(LogLevel::ERROR, "Cannot open fragment shader at " + std::string(fragmentPath));   
        if (geometryPath != nullptr) 
            Logger::Log(LogLevel::ERROR, "Cannot open geometry shader at " + std::string(geometryPath));
        return;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Етап №2: Компілюємо шейдери
    unsigned int vertex, fragment;

    // Вершиний шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Фрагментний шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int geometry;
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    if (vertex == 0 || fragment == 0) 
    {
        Logger::Log(LogLevel::ERROR, "Shader compilation failed, skipping program linking");
        ID = 0;
        return;
    }

    // Шейдерна программа
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    if (ID == 0) 
    { 
        Logger::Log(LogLevel::ERROR, "Shader program linking failed");
        return;
    }

    // Після того, як ми связали шейдери з нашою программою, видаляємо їх, так як вони нам більше не потрібні
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) 
        glDeleteShader(geometry);

    Logger::Log(LogLevel::INFO, "Shader program created with ID: " + std::to_string(ID));    
}
