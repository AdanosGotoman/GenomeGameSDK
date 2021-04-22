#pragma once
#include "ActorSystem.h"

namespace Genome
{
    class Entity;

    class GENOME_CLASS Actor : public ActorSystem
    {
    public:
        Actor();
        virtual ~Actor();

    public:
        void Create(Entity entity);
        void Remove(Entity entity);

        void OnRun();
        void OnStepMove();
    };
}


