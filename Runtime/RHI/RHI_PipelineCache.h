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

//= INCLUDES =====================
#include <memory>
#include <unordered_map>
#include "RHI_Definition.h"
#include "../Core/SpartanObject.h"
//================================

namespace Genome
{
    class RHI_PipelineCache : public SpartanObject
    {
    public:
        RHI_PipelineCache(const RHI_Device* rhi_device) { m_rhi_device = rhi_device; }
        RHI_Pipeline* GetPipeline(RHI_CommandList* cmd_list, RHI_PipelineState& pipeline_state, RHI_DescriptorSetLayout* descriptor_set_layout);

    private:
        // <hash of pipeline state, pipeline state object>
        std::unordered_map<uint32_t, std::shared_ptr<RHI_Pipeline>> m_cache;

        // Dependencies
        const RHI_Device* m_rhi_device;
    };
}
