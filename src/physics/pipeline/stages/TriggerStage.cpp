#include "TriggerStage.h"

#include "core/CommandManager.h"

void TriggerStage::Execute(StepContext &ctx) {
    for (auto &p: ctx.triggers)
        if (std::ranges::find(m_activeTriggers, p) == m_activeTriggers.end()) {
            CommandManager::ExecuteCommand("OnTriggerEnter", {p.a, p.b});
            Logger::Log(LogLevel::INFO, "Enter trigger");
        }

    for (auto &p: m_activeTriggers)
        if (std::ranges::find(ctx.triggers, p) == ctx.triggers.end()) {
            CommandManager::ExecuteCommand("OnTriggerExit", {p.a, p.b});
            Logger::Log(LogLevel::INFO, "Exit trigger");
        }

    m_activeTriggers = ctx.triggers;
}
