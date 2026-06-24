#pragma once

#include <angelscript.h>
#include <scriptbuilder.h>
#include <memory>
#include <vector>

#include "ECS/ECSModule.h"
#include "resource/ResourceModule.h"
#include "UI/UIModule.h"
#include "scene/SceneModule.h"
#include "rendering/primitive/PrimitivesFactory.h"
#include "ECS/components/Components.h"
#include "core/ModuleManager.h"

class EditorCommandHandler {
    ModuleManager *m_moduleManger;

    ECSModule *m_ecsModule;
    ResourceModule *m_resModule;
    UIModule *m_uiModule;
    SceneModule *m_sceneModule;

public:
    EditorCommandHandler(ModuleManager *mm);

    void RegisterAllCommands();

private:
    void RegisterObjectCommands();

    void RegisterLightCommands();

    void RegisterSceneCommands();

    void RegisterScriptCommands();
};