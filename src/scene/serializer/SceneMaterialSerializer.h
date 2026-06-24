#pragma once

#include <nlohmann/json.hpp>

#include "resource/material/MaterialManager.h"
#include "resource/material/Material.h"

using json = nlohmann::json;

class SceneMaterialSerializer {
public:
    json Serialize(MaterialManager *materialManager);

    void Deserialize(MaterialManager *materialManager, const json &data);
};