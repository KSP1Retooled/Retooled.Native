// Unity's Direct3D headers expect the Direct3D types to be declared already.
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>

#include "Gpu.h"
#include "IUnityGraphicsD3D11.h"
#include "IUnityGraphicsD3D12.h"

namespace retooled {
QueryResult
D3D12UnifiedMemory (IUnityInterfaces *interfaces)
{
	// Unity 6.7 doesn't register the first versions of the interface. UNITY_GET_INTERFACE ends
	// in a semicolon, so it can't be part of a larger expression.
	IUnityGraphicsD3D12v8 *d3d12 = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsD3D12v8);
	if (d3d12 == nullptr)
		return QueryResult::Unknown;

	ID3D12Device *device = d3d12->GetDevice ();
	if (device == nullptr)
		return QueryResult::Unknown;

	D3D12_FEATURE_DATA_ARCHITECTURE architecture{};
	if (FAILED (device->CheckFeatureSupport (D3D12_FEATURE_ARCHITECTURE, &architecture,
	                                         sizeof (architecture))))
		return QueryResult::Unknown;

	if (!architecture.UMA)
		return QueryResult::No;

	return QueryResult::Yes;
}

// Direct3D 11.3 reports it with the other D3D11_OPTIONS2 features, so it is unknown on Windows 8.1
// and older.
QueryResult
D3D11UnifiedMemory (IUnityInterfaces *interfaces)
{
	IUnityGraphicsD3D11 *d3d11 = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsD3D11);
	if (d3d11 == nullptr)
		return QueryResult::Unknown;

	ID3D11Device *device = d3d11->GetDevice ();
	if (device == nullptr)
		return QueryResult::Unknown;

	D3D11_FEATURE_DATA_D3D11_OPTIONS2 options{};
	if (FAILED (
			device->CheckFeatureSupport (D3D11_FEATURE_D3D11_OPTIONS2, &options, sizeof (options))))
		return QueryResult::Unknown;

	if (!options.UnifiedMemoryArchitecture)
		return QueryResult::No;

	return QueryResult::Yes;
}
} // namespace retooled
