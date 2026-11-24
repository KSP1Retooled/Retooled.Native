#pragma once

#include <cstdint>

struct IUnityInterfaces;

namespace retooled
{
    // Shared with C# as RetooledNative.Answer.
    enum class Answer : int32_t
    {
        Unknown = -1,
        No = 0,
        Yes = 1,
    };

    // Whether the GPU of Unity's device shares system memory, one function for each graphics API.
    Answer D3D11UnifiedMemory(IUnityInterfaces* interfaces);
    Answer D3D12UnifiedMemory(IUnityInterfaces* interfaces);
    Answer VulkanUnifiedMemory(IUnityInterfaces* interfaces);
    Answer MetalUnifiedMemory(IUnityInterfaces* interfaces);
}
