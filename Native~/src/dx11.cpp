#include <windows.h>
#include <d3d11.h>

#include "retooled.h"
#include "IUnityGraphicsD3D11.h"

namespace retooled {

QueryResult
HasUnifiedMemoryD3D11 (IUnityInterfaces *interfaces)
{
	auto iface = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsD3D11);
	if (iface == nullptr)
		return QueryResult::Unknown;

	auto device = iface->GetDevice ();
	if (device == nullptr)
		return QueryResult::Unknown;

	D3D11_FEATURE_DATA_D3D11_OPTIONS2 options{};
	if (FAILED (
			device->CheckFeatureSupport (D3D11_FEATURE_D3D11_OPTIONS2, &options, sizeof (options))))
		return QueryResult::Unknown;

	if (!options.UnifiedMemoryArchitecture)
		return QueryResult::False;
	return QueryResult::True;
}

} // namespace retooled
