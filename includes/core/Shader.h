#pragma once 

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // підключаємо glad для активації всіх заголовкових файлів OpenGL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader 
{
public:
    // ID - індефікатор програми
    unsigned int ID;

    // Конструктор зчитує данні й виконує будову шейдера
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    Shader(const char* shaderName, bool isGeometry = false);

    // Використання/активація шейдера
    void use();

    // Корисні unifirm-методи
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    // Векторні функції
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    
    // Матричні функції
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // Корисні мктоди для перевірки помилок компіляції/зв'язування шейдерів
    void checkCompileErrors(unsigned int shader, std::string type);   
    
    void CompileShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
};

#endif