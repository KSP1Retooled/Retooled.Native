#include "retooled.h"

// We use Unity's vulkan loader, so we don't need prototypes
#define VK_NO_PROTOTYPES
#define UNITY_VULKAN_HEADER <vulkan/vulkan.h>

#include "IUnityGraphics.h"
#include "IUnityGraphicsVulkan.h"

namespace retooled {

QueryResult
HasUnifiedMemoryVulkan (IUnityInterfaces *interfaces)
{
	auto iface = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsVulkan);
	if (iface == nullptr)
		return QueryResult::Unknown;

	auto instance = iface->Instance ();
	if (instance.physicalDevice == VK_NULL_HANDLE)
		return QueryResult::Unknown;

	auto getPropertiesFn = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties> (
		instance.getInstanceProcAddr (instance.instance, "vkGetPhysicalDeviceProperties"));
	if (getPropertiesFn == nullptr)
		return QueryResult::Unknown;

	VkPhysicalDeviceProperties properties;
	getPropertiesFn (instance.physicalDevice, &properties);

	switch (properties.deviceType) {
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return QueryResult::True;

	default:
		return QueryResult::False;
	}
}

} // namespace retooled
