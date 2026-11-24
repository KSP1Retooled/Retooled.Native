#import <Metal/Metal.h>

#include "Gpu.h"
#include "IUnityGraphicsMetal.h"

namespace retooled
{
    Answer MetalUnifiedMemory(IUnityInterfaces* interfaces)
    {
        IUnityGraphicsMetalV1* metal = UNITY_GET_INTERFACE(interfaces, IUnityGraphicsMetalV1);
        id<MTLDevice> device = metal != nullptr ? metal->MetalDevice() : nil;
        if (device == nil)
            return Answer::Unknown;

        return device.hasUnifiedMemory ? Answer::Yes : Answer::No;
    }
}
