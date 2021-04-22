#include "Spartan.h"
#include "Actor.h"

namespace Genome
{
    Actor::Actor()
    {
        m_speed = SPEED;
        m_health = HEALTH;
        m_maxHealth = HEALTH;

        npc.SetOrigin(0.0f, 0.0f, 0.0f);
    }

    void Actor::Create(World world, Genome::Math::Vector3 pos)
    {
        position.x = world.SetCoords(0.f, 0.f, 0.f);
        position.y = world.SetCoords(0.f, 0.f, 0.f);
        position.z = world.SetCoords(0.f, 0.f, 0.f);

        gameWorld->left = pos.left;
        gameWorld->right = pos.right;
        gameWorld->top = pos.top;
        gameWorld->bottom = pos.bottom;  
    }

    void Actor::OnRun()
    {
        
    }
}
