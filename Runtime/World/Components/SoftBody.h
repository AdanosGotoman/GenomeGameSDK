#pragma once

//= INCLUDES =====================
#include "IComponent.h"
#include "..\..\Math\Vector3.h"
#include "..\..\Math\Quaternion.h"
//================================

// = BULLET FORWARD DECLARATIONS =
class btSoftBody;
//================================

using namespace Genome::Math;

namespace Genome
{
    // = FORWARD DECLARATIONS =
    class Physics;
    //=========================

    class GENOME_CLASS SoftBody : public IComponent
    {
    public:
        SoftBody(Context* context, Entity* entity, uint32_t id = 0);
        ~SoftBody();

        //= ICOMPONENT ===============================
        void OnInitialize() override;
        void OnRemove() override;
        void OnStart() override;
        void OnTick(float delta_time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        //= POSITION =======================================
        Vector3 GetPosition() const;
        void SetPosition(const Vector3& position) const;
        //==================================================

        //= ROTATION ======================================
        Quaternion GetRotation() const;
        void SetRotation(const Quaternion& rotation) const;
        //=================================================

        void Activate() const;
        const Vector3& GetCenterOfMass() const { return m_center_of_mass; }

    private:
        void CreateBox();
        void CreateAeroCloth() const;

        void Body_Release();
        void Body_AddToWorld();
        void Body_RemoveFromWorld();

        Physics* m_physics        = nullptr;
        btSoftBody* m_soft_body   = nullptr;
        bool m_in_world           = false;
        Vector3 m_center_of_mass  = Vector3::Zero;
        float m_mass              = 0.0f;
    };
}
