#include <atomic>

#include "Gpu.h"
#include "IUnityGraphics.h"
#include "IUnityInterface.h"

namespace
{
    using retooled::Answer;

    // Marks an answer that hasn't been worked out for the current device yet.
    constexpr int32_t NotAsked = -2;

    IUnityInterfaces* interfaces;
    IUnityGraphics* graphics;

    // Worked out once for each Unity device.
    std::atomic<int32_t> unifiedMemory{NotAsked};

    void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType type)
    {
        if (type == kUnityGfxDeviceEventInitialize || type == kUnityGfxDeviceEventShutdown)
            unifiedMemory = NotAsked;
    }

    Answer AskUnifiedMemory()
    {
        switch (graphics->GetRenderer())
        {
#if defined(_WIN32)
        case kUnityGfxRendererD3D11:
            return retooled::D3D11UnifiedMemory(interfaces);
        case kUnityGfxRendererD3D12:
            return retooled::D3D12UnifiedMemory(interfaces);
#endif
#if defined(_WIN32) || defined(__linux__)
        case kUnityGfxRendererVulkan:
            return retooled::VulkanUnifiedMemory(interfaces);
#endif
#if defined(__APPLE__)
        case kUnityGfxRendererMetal:
            return retooled::MetalUnifiedMemory(interfaces);
#endif
        default:
            return Answer::Unknown;
        }
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    interfaces = unityInterfaces;
    graphics = UNITY_GET_INTERFACE(interfaces, IUnityGraphics);
    graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
    graphics = nullptr;
    interfaces = nullptr;
}

// An Answer: whether the GPU of Unity's graphics device shares system memory instead of having its own. Unknown
// before Unity has created its device, and on OpenGL, which doesn't say.
extern "C" int32_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RetooledNative_UnifiedMemory()
{
    if (graphics == nullptr || graphics->GetRenderer() == kUnityGfxRendererNull)
        return static_cast<int32_t>(Answer::Unknown);

    int32_t answer = unifiedMemory;
    if (answer == NotAsked)
    {
        answer = static_cast<int32_t>(AskUnifiedMemory());
        unifiedMemory = answer;
    }

    return answer;
}
