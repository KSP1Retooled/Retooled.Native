#include <optional>

#include "retooled.h"
#include "once.h"
#include "IUnityGraphics.h"
#include "IUnityInterface.h"

namespace retooled {
namespace {

struct QueryResults {
	Once<QueryResult> HasUnifiedMemory;

	QueryResults () = default;
};

static IUnityInterfaces *interfaces;
static IUnityGraphics *graphics;
static std::optional<QueryResults> results;

void UNITY_INTERFACE_API
OnGraphicsDeviceEvent (UnityGfxDeviceEventType type)
{
	switch (type) {
	case kUnityGfxDeviceEventInitialize:
		results = QueryResults ();
		break;
	case kUnityGfxDeviceEventShutdown:
		results = std::nullopt;
		break;

	default:
		break;
	}
}

QueryResult
HasUnifiedMemory ()
{
	if (!results)
		return QueryResult::Unknown;
	if (!graphics || !interfaces)
		return QueryResult::Unknown;

	return results->HasUnifiedMemory.get_or_init ([] () {
		switch (graphics->GetRenderer ()) {
#if defined(_WIN32)
		case kUnityGfxRendererD3D11:
			return HasUnifiedMemoryD3D11 (interfaces);
		case kUnityGfxRendererD3D12:
			return HasUnifiedMemoryD3D12 (interfaces);
#endif

#if defined(_WIN32) || defined(__linux__)
		case kUnityGfxRendererVulkan:
			return HasUnifiedMemoryVulkan (interfaces);
#endif

#if defined(__APPLE__)
		case kUnityGfxRendererMetal:
			return HasUnifiedMemoryMetal (interfaces);
#endif

		default:
			return QueryResult::Unknown;
		}
	});
}

} // namespace
} // namespace retooled

extern "C" void RETOOLED_API
UnityPluginLoad (IUnityInterfaces *unityInterfaces)
{
	retooled::interfaces = unityInterfaces;
	retooled::graphics = UNITY_GET_INTERFACE (unityInterfaces, IUnityGraphics);
	retooled::graphics->RegisterDeviceEventCallback (retooled::OnGraphicsDeviceEvent);
	retooled::results = retooled::QueryResults ();
}

extern "C" void RETOOLED_API
UnityPluginUnload ()
{
	retooled::graphics->UnregisterDeviceEventCallback (retooled::OnGraphicsDeviceEvent);
	retooled::graphics = nullptr;
	retooled::interfaces = nullptr;
	retooled::results = std::nullopt;
}

extern "C" retooled::QueryResult RETOOLED_API
Retooled_HasUnifiedMemory ()
{
	return retooled::HasUnifiedMemory ();
}
