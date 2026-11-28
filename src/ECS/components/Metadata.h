#pragma once

#include <string>
#include <utility>

struct c_Tag {
    std::string name;

    c_Tag() = default;

    explicit c_Tag(std::string n) : name(std::move(n)) {
    }
};

struct c_ID {
    uint64_t id;

    c_ID() = default;

    explicit c_ID(const uint64_t i) : id(i) {}

    bool operator!=(const c_ID &other) const { return id != other.id; }
};

/// @brief Tag: the entity survives ECSWorld::Clear() (scene load / new scene)
struct c_Persistent {
};

struct c_Model {
    std::string filePath;

    c_Model() = default;

    explicit c_Model(std::string path) : filePath(std::move(path)) {}
};