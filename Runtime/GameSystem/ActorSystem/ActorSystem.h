#pragma once
#include "../World/Entity.h"

namespace Genome
{
    class Entity;

    class GENOME_CLASS ActorSystem : public Entity
    {
    public:
        ActorSystem();
        virtual ~ActorSystem();

    public:
        void Create(Entity* entity);
        void Remove(Entity* entity);

        void OnRun();
        void OnStepMove();
    };
}
