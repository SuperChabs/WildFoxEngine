module;

#include <vector>
#include <algorithm>
#include <variant>
#include <set>
#include <utility>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

export module WFE.ECS.Systems.PhysicsSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;
import WFE.Core.Logger;
import WFE.Core.CommandManager;

struct ContactInfo
{
    entt::entity a;
    entt::entity b;
    glm::vec3 normal;
    float depth;
};

export class PhysicsSystem
{
    glm::vec3 gravity = {0, -9.8, 0};
    std::vector<entt::entity> entities;

    std::set<std::pair<entt::entity, entt::entity>> m_activeTriggers;

public:
    void Update(ECSWorld& world, float dt)
    {
        entities.clear();

        world.Each<TransformComponent, RigidBodyComponent, ColliderComponent>(
            [&](entt::entity entity,
            TransformComponent& t,
            RigidBodyComponent& r,
            ColliderComponent&  c)
        {
            if (r.inv_mass != 0)
            {
                r.velocity += gravity * dt;
                t.position += r.velocity * dt;
            }
            
            entities.push_back(entity);
        });

        std::set<std::pair<entt::entity, entt::entity>> currentTriggers;

        for (int i = 0; i < entities.size(); i++)
            for (int j = i + 1; j < entities.size(); j++)
            {
                auto& col_a = world.GetComponent<ColliderComponent>(entities[i]);
                auto& col_b = world.GetComponent<ColliderComponent>(entities[j]);
                auto& rb_a  = world.GetComponent<RigidBodyComponent>(entities[i]);
                auto& rb_b  = world.GetComponent<RigidBodyComponent>(entities[j]);
                auto& t_a = world.GetComponent<TransformComponent>(entities[i]);
                auto& t_b = world.GetComponent<TransformComponent>(entities[j]);

                if (!std::holds_alternative<AABB>(col_a.shape) && 
                    !std::holds_alternative<AABB>(col_b.shape))
                    continue;

                AABB& aabb_a = std::get<AABB>(col_a.shape);
                AABB& aabb_b = std::get<AABB>(col_b.shape);
            
                ContactInfo contact{};
                contact.a = entities[i];
                contact.b = entities[j];
                
                AABB world_a = {
                    aabb_a.min + t_a.position,
                    aabb_a.max + t_a.position
                };
                AABB world_b = {
                    aabb_b.min + t_b.position,
                    aabb_b.max + t_b.position
                };

                if (!TestAABB(world_a, world_b, contact))
                    continue;

                if (col_a.isTrigger || col_b.isTrigger)
                {
                    auto pair = std::make_pair(entities[i], entities[j]);
                    currentTriggers.insert(pair);
                }
                else 
                {   
                    float total = rb_a.inv_mass + rb_b.inv_mass;
                    if (total == 0.0f) continue;

                    float vRel = glm::dot(rb_a.velocity - rb_b.velocity, contact.normal);
                    if (vRel > 0.0f)
                    {
                        rb_a.velocity -= contact.normal * vRel * (rb_a.inv_mass / total);
                        rb_b.velocity += contact.normal * vRel * (rb_b.inv_mass / total);
                    }

                    float ratio_a = rb_a.inv_mass / total;
                    float ratio_b = rb_b.inv_mass / total;
                    t_a.position -= contact.normal * contact.depth * ratio_a;
                    t_b.position += contact.normal * contact.depth * ratio_b;
                }
            }

        for (auto& p : currentTriggers)
            if (!m_activeTriggers.count(p))
            {    
                CommandManager::ExecuteCommand("OnTriggerEnter", {p.first, p.second});
                Logger::Log(LogLevel::INFO, "Enter trigger");
            }

        for (auto& p : m_activeTriggers)
            if (!currentTriggers.count(p))
            {    
                CommandManager::ExecuteCommand("OnTriggerExit", {p.first, p.second});
                Logger::Log(LogLevel::INFO, "Enter trigger");
            }

        m_activeTriggers = currentTriggers;
    }

    glm::vec3 GetGravity() { return gravity; }
    void SetGravity(glm::vec3 newGravity) { gravity = newGravity; }

private:
    bool TestAABB(const AABB& a, const AABB& b, ContactInfo& contact)
    {
        float ox = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
        float oy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
        float oz = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

        if (ox <= 0 || oy <= 0 || oz <= 0) return false;

        glm::vec3 centerA = (a.min + a.max) * 0.5f;
        glm::vec3 centerB = (b.min + b.max) * 0.5f;
        glm::vec3 dir = centerB - centerA;

        if (ox < oy && ox < oz)
        {
            contact.depth = ox;
            contact.normal = (dir.x > 0) ? glm::vec3(1,0,0) : glm::vec3(-1,0,0);
        }
        else if (oy < oz)
        {
            contact.depth = oy;
            contact.normal = (dir.y > 0) ? glm::vec3(0,1,0) : glm::vec3(0,-1,0);
        }
        else
        {
            contact.depth = oz;
            contact.normal = (dir.z > 0) ? glm::vec3(0,0,1) : glm::vec3(0,0,-1);
        }

        return true;
    }
};