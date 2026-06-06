#pragma once

#include <string>
#include <cstdint>

struct TagComponent {
    std::string name;

    TagComponent() = default;

    TagComponent(const std::string &n) : name(n) {
    }
};

struct IDComponent {
    uint64_t id;

    IDComponent() = default;

    IDComponent(uint64_t i) : id(i) {
    }
};

struct CameraTypeComponent {
    enum class Type { EDITOR, GAME };

    Type type = Type::EDITOR;

    CameraTypeComponent() = default;

    CameraTypeComponent(Type t) : type(t) {
    }
};

struct ModelComponent {
    std::string filePath;

    ModelComponent() = default;

    ModelComponent(const std::string &path) : filePath(path) {
    }
};