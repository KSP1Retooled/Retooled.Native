#pragma once

#include <cstdint>

struct IUnityInterfaces;

namespace retooled {
// Shared with C# as RetooledNative.QueryResult.
enum class QueryResult : int32_t {
	Unknown = -1,
	No = 0,
	Yes = 1,
};

// Whether the GPU of Unity's device shares system memory, one function for each graphics API.
QueryResult D3D11UnifiedMemory (IUnityInterfaces *interfaces);
QueryResult D3D12UnifiedMemory (IUnityInterfaces *interfaces);
QueryResult VulkanUnifiedMemory (IUnityInterfaces *interfaces);
QueryResult MetalUnifiedMemory (IUnityInterfaces *interfaces);
QueryResult OpenGLUnifiedMemory ();
} // namespace retooled
