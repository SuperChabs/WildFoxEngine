#pragma once

#include <string>
#include <cstdint>
#include <utility>

struct TagComponent {
    std::string name;

    TagComponent() = default;

    explicit TagComponent(std::string n) : name(std::move(n)) {}
};

struct IDComponent {
    uint64_t id;

    IDComponent() = default;

    explicit IDComponent(const uint64_t i) : id(i) {}
};

struct ModelComponent {
    std::string filePath;

    ModelComponent() = default;

    explicit ModelComponent(std::string path) : filePath(std::move(path)) {}
};