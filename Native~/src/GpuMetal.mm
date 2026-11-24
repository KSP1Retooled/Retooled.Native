// Metal's API is Objective-C, and so is Unity's IUnityGraphicsMetal.h, so this file is
// Objective-C++.
#import <Metal/Metal.h>

#include "Gpu.h"
#include "IUnityGraphicsMetal.h"

namespace retooled {
QueryResult
MetalUnifiedMemory (IUnityInterfaces *interfaces)
{
	IUnityGraphicsMetalV1 *metal = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsMetalV1);
	if (metal == nullptr)
		return QueryResult::Unknown;

	id<MTLDevice> device = metal->MetalDevice ();
	if (device == nil)
		return QueryResult::Unknown;

	if (!device.hasUnifiedMemory)
		return QueryResult::No;

	return QueryResult::Yes;
}
} // namespace retooled
