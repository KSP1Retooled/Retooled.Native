#pragma once

#include <cstdint>
#include "IUnityInterface.h"

#define RETOOLED_API UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API

namespace retooled {

// Shared with C# as RetooledNative.QueryResult.
enum class QueryResult : int32_t {
	Unknown = -1,
	False = 0,
	True = 1,
};

#if defined(_WIN32)
QueryResult HasUnifiedMemoryD3D11 (IUnityInterfaces *interfaces);
QueryResult HasUnifiedMemoryD3D12 (IUnityInterfaces *interfaces);
#endif

#if defined(_WIN32) || defined(__linux__)
QueryResult HasUnifiedMemoryVulkan (IUnityInterfaces *interfaces);
#endif

#if defined(__APPLE__)
QueryResult HasUnifiedMemoryMetal (IUnityInterfaces *interfaces);
#endif

} // namespace retooled

extern "C" retooled::QueryResult RETOOLED_API Retooled_HasUnifiedMemory ();
