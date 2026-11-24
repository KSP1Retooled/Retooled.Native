#include <atomic>

#include "Gpu.h"
#include "IUnityGraphics.h"
#include "IUnityInterface.h"

namespace retooled {
namespace {
// Marks a result that hasn't been worked out for the current device yet.
constexpr int32_t NotAsked = -2;

IUnityInterfaces *interfaces;
IUnityGraphics *graphics;

// Worked out once for each Unity device.
std::atomic<int32_t> unifiedMemory{NotAsked};

void UNITY_INTERFACE_API
OnGraphicsDeviceEvent (UnityGfxDeviceEventType type)
{
	if (type == kUnityGfxDeviceEventInitialize || type == kUnityGfxDeviceEventShutdown)
		unifiedMemory = NotAsked;
}

QueryResult
QueryUnifiedMemory ()
{
	switch (graphics->GetRenderer ()) {
#if defined(_WIN32)
	case kUnityGfxRendererD3D11:
		return D3D11UnifiedMemory (interfaces);
	case kUnityGfxRendererD3D12:
		return D3D12UnifiedMemory (interfaces);
#endif
#if defined(_WIN32) || defined(__linux__)
	case kUnityGfxRendererVulkan:
		return VulkanUnifiedMemory (interfaces);
#endif
#if defined(__linux__)
	case kUnityGfxRendererOpenGLCore:
		return OpenGLUnifiedMemory ();
#endif
#if defined(__APPLE__)
	case kUnityGfxRendererMetal:
		return MetalUnifiedMemory (interfaces);
#endif
	default:
		return QueryResult::Unknown;
	}
}
} // namespace

QueryResult
UnifiedMemory ()
{
	if (graphics == nullptr || graphics->GetRenderer () == kUnityGfxRendererNull)
		return QueryResult::Unknown;

	int32_t result = unifiedMemory;
	if (result != NotAsked)
		return static_cast<QueryResult> (result);

	QueryResult queried = QueryUnifiedMemory ();
	unifiedMemory = static_cast<int32_t> (queried);
	return queried;
}
} // namespace retooled

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad (IUnityInterfaces *unityInterfaces)
{
	retooled::interfaces = unityInterfaces;
	retooled::graphics = UNITY_GET_INTERFACE (unityInterfaces, IUnityGraphics);
	retooled::graphics->RegisterDeviceEventCallback (retooled::OnGraphicsDeviceEvent);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload ()
{
	retooled::graphics->UnregisterDeviceEventCallback (retooled::OnGraphicsDeviceEvent);
	retooled::graphics = nullptr;
	retooled::interfaces = nullptr;
}

// A QueryResult: whether the GPU of Unity's graphics device shares system memory instead of
// having its own. Unknown before Unity has created its device, and where the graphics API or
// driver doesn't say.
extern "C" int32_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
RetooledNative_UnifiedMemory ()
{
	return static_cast<int32_t> (retooled::UnifiedMemory ());
}
