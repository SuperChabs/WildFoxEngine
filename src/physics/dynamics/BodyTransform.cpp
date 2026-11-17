#include "BodyTransform.h"

glm::vec3 BodyTransform::WorldSpaceToBodySpace(const BodyInfo &body, const glm::vec3 &worldPt) {
    const glm::vec3 tmp = glm::inverse(body.transform->rotation) * (worldPt - GetCenterOfMassWorldSpace(body));
    return tmp / body.transform->scale;
}

glm::vec3 BodyTransform::BodySpaceToWorldSpace(const BodyInfo &body, const glm::vec3 &worldPt) {
    return GetCenterOfMassWorldSpace(body) + body.transform->rotation * (worldPt * body.transform->scale);
}

glm::vec3 BodyTransform::GetCenterOfMassWorldSpace(const BodyInfo &body) {
    const glm::vec3 centerOfMass = std::visit([](const auto& shape) {
            return shape.m_centerOfMass;
        }, body.collider->shape);
    const glm::vec3 pos = body.transform->position + body.transform->rotation * (centerOfMass * body.transform->scale);
    return pos;
}
