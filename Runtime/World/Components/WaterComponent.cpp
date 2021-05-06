#include "Spartan.h"
#include "WaterComponent.h"
#include "Renderable.h"
#include "..\Entity.h"
#include "..\..\RHI\RHI_Texture2D.h"
#include "..\..\RHI\RHI_Vertex.h"
#include "..\..\Rendering\Model.h"
#include "..\..\IO\FileStream.h"
#include "..\..\Resource\ResourceCache.h"
#include "..\..\Rendering\Mesh.h"
#include "..\..\Threading\Threading.h"

using namespace Genome::Math;

namespace Genome
{
    WaterComponent::WaterComponent(Context* context, Entity* entity, uint32_t id /*=0*/) : IComponent(context, entity, id)
    {

    }

    void WaterComponent::OnInitialize()
    {

    }

    void WaterComponent::Serialize(FileStream* stream)
    {
        
    }

    void WaterComponent::Deserialize(FileStream* stream)
    {
        
    }


}
