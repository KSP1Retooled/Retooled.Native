// Unity's Direct3D headers expect the Direct3D and DXGI types to be declared already.
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>

#include "Gpu.h"
#include "IUnityGraphicsD3D11.h"
#include "IUnityGraphicsD3D12.h"

namespace
{
    retooled::Answer Uma(ID3D12Device* device)
    {
        D3D12_FEATURE_DATA_ARCHITECTURE architecture{};
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture))))
            return retooled::Answer::Unknown;

        return architecture.UMA ? retooled::Answer::Yes : retooled::Answer::No;
    }
}

namespace retooled
{
    Answer D3D12UnifiedMemory(IUnityInterfaces* interfaces)
    {
        // Unity 6.7 doesn't register the first versions of the interface. UNITY_GET_INTERFACE ends in a semicolon, so
        // it can't be part of a larger expression.
        IUnityGraphicsD3D12v8* d3d12 = UNITY_GET_INTERFACE(interfaces, IUnityGraphicsD3D12v8);
        ID3D12Device* device = d3d12 != nullptr ? d3d12->GetDevice() : nullptr;
        return device != nullptr ? Uma(device) : Answer::Unknown;
    }

    // Direct3D 11.3 reports it with the other D3D11_OPTIONS2 features, so it is unknown on Windows 8.1 and older.
    Answer D3D11UnifiedMemory(IUnityInterfaces* interfaces)
    {
        IUnityGraphicsD3D11* d3d11 = UNITY_GET_INTERFACE(interfaces, IUnityGraphicsD3D11);
        ID3D11Device* device = d3d11 != nullptr ? d3d11->GetDevice() : nullptr;
        if (device == nullptr)
            return Answer::Unknown;

        D3D11_FEATURE_DATA_D3D11_OPTIONS2 options{};
        if (FAILED(device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &options, sizeof(options))))
            return Answer::Unknown;

        return options.UnifiedMemoryArchitecture ? Answer::Yes : Answer::No;
    }
}
