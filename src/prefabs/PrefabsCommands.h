#pragma once

#include "ECS/ECSModule.h"
#include "resource/ResourceModule.h"

class PrefabsCommands {
public:
    static void RegisterPrefabsCommands(ECSModule *ecsModule, ResourceModule *resModule);
};
