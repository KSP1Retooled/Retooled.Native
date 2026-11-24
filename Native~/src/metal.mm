
#import <Metal/Metal.h>

#include "retooled.h"
#include "IUnityGraphicsMetal.h"

namespace retooled {

QueryResult
HasUnifiedMemoryMetal (IUnityInterfaces *interfaces)
{
	IUnityGraphicsMetalV1 *metal = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsMetalV1);
	if (metal == nullptr)
		return QueryResult::Unknown;

	id<MTLDevice> device = metal->MetalDevice ();
	if (device == nil)
		return QueryResult::Unknown;

	if (device.hasUnifiedMemory)
		return QueryResult::True;
	return QueryResult::False;
}

} // namespace retooled
