#include "Spartan.h"
#include "RHI_Device.h"
#include "RHI_Implementation.h"
//=============================

//= NAMESPACES ===============
using namespace std;
using namespace Genome::Math;
//============================

namespace Genome
{
    void RHI_Device::RegisterPhysicalDevice(const PhysicalDevice& physical_device)
    {
        m_physical_devices.emplace_back(physical_device);

        // Keep devices sorted, based on memory (from highest to lowest)
        sort(m_physical_devices.begin(), m_physical_devices.end(), [](const PhysicalDevice& adapter1, const PhysicalDevice& adapter2)
            {
                return adapter1.GetMemory() > adapter2.GetMemory();
            });

        LOG_INFO("%s (%d MB)", physical_device.GetName().c_str(), physical_device.GetMemory());
    }

    const PhysicalDevice* RHI_Device::GetPrimaryPhysicalDevice()
    {
        if (m_physical_device_index >= m_physical_devices.size())
            return nullptr;

        return &m_physical_devices[m_physical_device_index];
    }

    void RHI_Device::SetPrimaryPhysicalDevice(const uint32_t index)
    {
        m_physical_device_index = index;

        if (const PhysicalDevice* physical_device = GetPrimaryPhysicalDevice())
        {
            LOG_INFO("%s (%d MB)", physical_device->GetName().c_str(), physical_device->GetMemory());
        }
    }

    bool RHI_Device::IsValidResolution(const uint32_t width, const uint32_t height)
    {
        return  width > 4 && width <= RHI_Context::texture_2d_dimension_max &&
            height > 4 && height <= RHI_Context::texture_2d_dimension_max;
    }

    bool RHI_Device::Queue_WaitAll() const
    {
        return Queue_Wait(RHI_Queue_Graphics) && Queue_Wait(RHI_Queue_Transfer) && Queue_Wait(RHI_Queue_Compute);
    }

    void* RHI_Device::Queue_Get(const RHI_Queue_Type type) const
    {
        if (type == RHI_Queue_Graphics)
        {
            return m_rhi_context->queue_graphics;
        }
        else if (type == RHI_Queue_Transfer)
        {
            return m_rhi_context->queue_transfer;
        }
        else if (type == RHI_Queue_Compute)
        {
            return m_rhi_context->queue_compute;
        }

        return nullptr;
    }

    uint32_t RHI_Device::Queue_Index(const RHI_Queue_Type type) const
    {
        if (type == RHI_Queue_Graphics)
        {
            return m_rhi_context->queue_graphics_index;
        }
        else if (type == RHI_Queue_Transfer)
        {
            return m_rhi_context->queue_transfer_index;
        }
        else if (type == RHI_Queue_Compute)
        {
            return m_rhi_context->queue_compute_index;
        }

        return 0;
    }
}
