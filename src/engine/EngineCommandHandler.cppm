export module WFE.Engine.EditorCommandHandler;

import WFE.ECS.ECSModule;
import WFE.Resource.ResourceModule;
import WFE.UI.UIModule;

export class EditorCommandHandler
{
    ECSModule* ecsModule;
    ResourceModule* resModule;
    UIModule* uiModule;

public:
    EditorCommandHandler(ECSModule* ecs, ResourceModule* res, UIModule* ui)
        : ecsModule(ecs), resModule(res), uiModule(ui)
    {
    }
};