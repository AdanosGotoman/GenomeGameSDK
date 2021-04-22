#pragma once
#include "../Math/Vector3.h"
#include "assimp/scene.h"
#include "../World/World.h"

namespace Genome
{
    class Entity;

    enum eStates { state_STAND, stateJUMP, stateMOVING};

    class GENOME_CLASS Actor
    {
    private:
        const float SPEED = 200;
        const float HEALTH = 100;

        Genome::Math::Vector3 position;
        Genome::Math::Sphere npc;

        //aiScene* scene;
        World* gameWorld;

        bool up;
        bool down;
        bool left;
        bool right;

        int m_health;
        int m_maxHealth;

        float m_speed;

    public:
        Actor();
        virtual ~Actor();

    public:
        void Create(World world, Genome::Math::Vector3 pos);
        void Remove();

        void OnRun();
        void OnStepMove();

    public:
        virtual void SetName(const char* name) = 0;
        virtual const char* GetName() const = 0;
    };
}


