module;

#include "ext/stdlib.hpp"
#include "ext/entt.hpp"
#include "ext/glm.hpp"

export module WFE.ECS.Systems.PhysicsSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;

export class PhysicsSystem
{
    glm::vec3 gravity = {0, -9.8, 0};

public:
    void Update(ECSWorld& world, float dt)
    {
        world.Each<TransformComponent, RigidBodyComponent>(
            [&](entt::entity entity,
            TransformComponent& t,
            RigidBodyComponent& r)
        {
            if (r.inv_mass == 0) return;

            if (t.position.y > 0)
            {            
                r.velocity += gravity * dt;
                t.position += r.velocity * dt;
            }
        });
    }

    glm::vec3 GetGravity() { return gravity; }
    void SetGravity(glm::vec3 newGravity) { gravity = newGravity; }

private:
    bool TestAABB(const AABB& a, const AABB& b)
    {
        bool overlap_x = a.max.x >= b.min.x && b.max.x >= a.min.x;
        bool overlap_y = a.max.y >= b.min.y && b.max.y >= a.min.y;
        bool overlap_z = a.max.z >= b.min.z && b.max.z >= a.min.z;

        return overlap_x && overlap_y && overlap_z;
    }
};