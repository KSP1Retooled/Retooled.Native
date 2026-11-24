#include <windows.h>
#include <dxgi.h>
#include <d3d12.h>

#include "retooled.h"
#include "IUnityGraphicsD3D12.h"

namespace retooled {

QueryResult
HasUnifiedMemoryD3D12 (IUnityInterfaces *interfaces)
{
	auto iface = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsD3D12v8);
	if (iface == nullptr)
		return QueryResult::Unknown;

	auto device = iface->GetDevice ();
	if (device == nullptr)
		return QueryResult::Unknown;

	D3D12_FEATURE_DATA_ARCHITECTURE arch{};
	if (FAILED (device->CheckFeatureSupport (D3D12_FEATURE_ARCHITECTURE, &arch, sizeof (arch))))
		return QueryResult::Unknown;

	if (!arch.UMA)
		return QueryResult::False;
	return QueryResult::True;
}

} // namespace retooled
