/*
Copyright(c) 2016-2021 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ==================
#include "IComponent.h"
#include <vector>
#include "../../Math/Vector3.h"
//=============================

class btRigidBody;
class btCollisionShape;

using namespace Genome::Math;

namespace Genome
{
    class Entity;
    class Constraint;
    class Physics;
    namespace Math { class Quaternion; }

    enum ForceMode
    {
        Force,
        Impulse
    };

    class GENOME_CLASS RigidBody : public IComponent
    {
    public:
        RigidBody(Context* context, Entity* entity, uint32_t id = 0);
        ~RigidBody();

        //= ICOMPONENT ===============================
        void OnInitialize() override;
        void OnRemove() override;
        void OnStart() override;
        void OnTick(float delta_time) override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

        //= MASS =========================
        float GetMass() const { return m_mass; }
        void SetMass(float mass);
        //================================

        //= DRAG =================================
        float GetFriction() const { return m_friction; }
        void SetFriction(float friction);
        //========================================

        //= ANGULAR DRAG =======================================
        float GetFrictionRolling() const { return m_friction_rolling; }
        void SetFrictionRolling(float frictionRolling);
        //======================================================

        //= RESTITUTION ================================
        float GetRestitution() const { return m_restitution; }
        void SetRestitution(float restitution);
        //==============================================

        //= GRAVITY =======================================
        void SetUseGravity(bool gravity);
        bool GetUseGravity() const { return m_use_gravity; };
        Vector3 GetGravity() const { return m_gravity; }
        void SetGravity(const Vector3& acceleration);
        //=================================================

        //= KINEMATIC ===============================
        void SetIsKinematic(bool kinematic);
        bool GetIsKinematic() const { return m_is_kinematic; }
        //===========================================

        //= VELOCITY/FORCE/TORQUE ==========================================================================
        void SetLinearVelocity(const Vector3& velocity, const bool activate = true) const;
        void SetAngularVelocity(const Vector3& velocity, const bool activate = true) const;
        void ApplyForce(const Vector3& force, ForceMode mode) const;
        void ApplyForceAtPosition(const Vector3& force, const Vector3& position, ForceMode mode) const;
        void ApplyTorque(const Vector3& torque, ForceMode mode) const;
        //==================================================================================================

        //= POSITION LOCK ========================================
        void SetPositionLock(bool lock);
        void SetPositionLock(const Vector3& lock);
        Vector3 GetPositionLock() const { return m_position_lock; }
        //========================================================

        //= ROTATION LOCK ========================================
        void SetRotationLock(bool lock);
        void SetRotationLock(const Vector3& lock);
        Vector3 GetRotationLock() const { return m_rotation_lock; }
        //========================================================

        //= CENTER OF MASS ==============================================
        void SetCenterOfMass(const Vector3& centerOfMass);
        const Vector3& GetCenterOfMass() const { return m_center_of_mass; }
        //===============================================================

        //= POSITION ===============================================================
        Vector3 GetPosition() const;
        void SetPosition(const Vector3& position, const bool activate = true) const;
        //==========================================================================

        //= ROTATION ==================================================================
        Quaternion GetRotation() const;
        void SetRotation(const Quaternion& rotation, const bool activate = true) const;
        //=============================================================================

        //= MISC ============================================
        void ClearForces() const;
        void Activate() const;
        void Deactivate() const;
        btRigidBody* GetBtRigidBody() const { return m_rigidBody; }
        bool IsInWorld() const { return m_in_world; }
        //===================================================

        // Communication with other physics components
        void AddConstraint(Constraint* constraint);
        void RemoveConstraint(Constraint* constraint);
        void SetShape(btCollisionShape* shape);

    private:
        void Body_AddToWorld();
        void Body_Release();
        void Body_RemoveFromWorld();
        void Body_AcquireShape();
        void Flags_UpdateKinematic() const;
        void Flags_UpdateGravity() const;
        bool IsActivated() const;

        float m_mass              = 0.0f;
        float m_friction          = 0.0f;
        float m_friction_rolling  = 0.0f;
        float m_restitution       = 0.0f;
        bool m_use_gravity        = false;
        bool m_is_kinematic       = false;
        Vector3 m_gravity         = Vector3::Zero;
        Vector3 m_position_lock   = Vector3::Zero;
        Vector3 m_rotation_lock   = Vector3::Zero;
        Vector3 m_center_of_mass  = Vector3::Zero;

        btRigidBody* m_rigidBody            = nullptr;
        btCollisionShape* m_collision_shape = nullptr;
        bool m_in_world                     = false;
        Physics* m_physics                  = nullptr;
        std::vector<Constraint*> m_constraints;
    };
}
