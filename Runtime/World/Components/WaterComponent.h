#pragma once
#include "IComponent.h"
#include "..\..\RHI\RHI_Implementation.h"
#include <atomic>

namespace Genome
{
    class GENOME_CLASS WaterComponent : public IComponent
    {
    public:
        WaterComponent(Context* context, Entity* entity, uint32_t id = 0);
        ~WaterComponent() = default;

        //= IComponent ===============================
        void OnInitialize() override;
        void Serialize(FileStream* stream) override;
        void Deserialize(FileStream* stream) override;
        //============================================

    public:

    };
}


