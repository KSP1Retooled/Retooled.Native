// The plug-in calls Vulkan through the loader that Unity hands it, so it needs no prototypes.
#define VK_NO_PROTOTYPES
#define UNITY_VULKAN_HEADER <vulkan/vulkan.h>

#include "Gpu.h"
// IUnityGraphicsVulkan.h uses the rendering event types without including their header.
#include "IUnityGraphics.h"
#include "IUnityGraphicsVulkan.h"

namespace retooled {
QueryResult
VulkanUnifiedMemory (IUnityInterfaces *interfaces)
{
	IUnityGraphicsVulkan *vulkan = UNITY_GET_INTERFACE (interfaces, IUnityGraphicsVulkan);
	if (vulkan == nullptr)
		return QueryResult::Unknown;

	UnityVulkanInstance instance = vulkan->Instance ();
	if (instance.physicalDevice == VK_NULL_HANDLE)
		return QueryResult::Unknown;

	auto getProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties> (
		instance.getInstanceProcAddr (instance.instance, "vkGetPhysicalDeviceProperties"));
	if (getProperties == nullptr)
		return QueryResult::Unknown;

	VkPhysicalDeviceProperties properties;
	getProperties (instance.physicalDevice, &properties);

	// A software renderer such as llvmpipe also keeps its textures in system memory.
	if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
	    && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU)
		return QueryResult::No;

	return QueryResult::Yes;
}
} // namespace retooled
