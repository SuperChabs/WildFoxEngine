#pragma once

enum class HoveredZone {
    None,
    Viewport,
    Hierarchy,
    Inspector,
    Timeline
};

class UIInput {
    HoveredZone hoveredZone = HoveredZone::None;

public:
    void SetHoveredZone(HoveredZone zone) { hoveredZone = zone; }
    void ResetZone() { hoveredZone = HoveredZone::None; }
    HoveredZone GetHoveredZone() const { return hoveredZone; }
};