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

//= INCLUDES =====================
#include "Spartan.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_IndexBuffer.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Genome
{
    void RHI_IndexBuffer::_destroy()
    {
        d3d11_utility::release(*reinterpret_cast<ID3D11Buffer**>(&m_buffer));
    }

    bool RHI_IndexBuffer::_create(const void* indices)
    {
        if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device)
        {
            LOG_ERROR_INVALID_INTERNALS();
            return false;
        }

        const bool is_dynamic = indices == nullptr;

        // Destroy previous buffer
        _destroy();

        D3D11_BUFFER_DESC buffer_desc;
        ZeroMemory(&buffer_desc, sizeof(buffer_desc));
        buffer_desc.ByteWidth           = m_stride * m_index_count;
        buffer_desc.Usage               = is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
        buffer_desc.CPUAccessFlags      = is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
        buffer_desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        buffer_desc.MiscFlags           = 0;
        buffer_desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA init_data    = {};
        init_data.pSysMem                   = indices;
        init_data.SysMemPitch               = 0;
        init_data.SysMemSlicePitch          = 0;

        const auto ptr = reinterpret_cast<ID3D11Buffer**>(&m_buffer);
        const auto result = m_rhi_device->GetContextRhi()->device->CreateBuffer(&buffer_desc, is_dynamic ? nullptr : &init_data, ptr);
        if FAILED(result)
        {
            LOG_ERROR(" Failed to create index buffer");
            return false;
        }

        return true;
    }

    void* RHI_IndexBuffer::Map()
    {
        if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device_context || !m_buffer)
        {
            LOG_ERROR_INVALID_INTERNALS();
            return nullptr;
        }

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        const auto result = m_rhi_device->GetContextRhi()->device_context->Map(static_cast<ID3D11Resource*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        if (FAILED(result))
        {
            LOG_ERROR("Failed to map index buffer.");
            return nullptr;
        }

        return mapped_resource.pData;
    }

    bool RHI_IndexBuffer::Unmap()
    {
        if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device_context || !m_buffer)
        {
            LOG_ERROR_INVALID_INTERNALS();
            return false;
        }

        m_rhi_device->GetContextRhi()->device_context->Unmap(static_cast<ID3D11Resource*>(m_buffer), 0);
        return true;
    }
}
