#pragma once

#include <angelscript.h>
#include <entt/entt.hpp>

#include <string>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/CommandManager.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

inline void CreateCube(glm::vec3 &pos, glm::vec3 &rot, glm::vec3 &scale) {
}

inline void RegisterExperimentalStuff(asIScriptEngine *engine) {
}