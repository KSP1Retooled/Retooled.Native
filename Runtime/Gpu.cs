namespace Retooled.Native;

/// <summary>Facts about the GPU that Unity renders with, which Unity's own API doesn't give.</summary>
public static class Gpu
{
    /// <summary>
    /// Whether the GPU shares system memory instead of having VRAM of its own, as integrated GPUs and Apple's GPUs do.
    /// On such GPUs, <see cref="UnityEngine.SystemInfo.graphicsMemorySize"/> can count most of system memory as VRAM.
    /// </summary>
    ///
    /// <remarks>
    /// Null before Unity has created its graphics device, and where the graphics API or driver doesn't say. Direct3D
    /// and Metal report it directly. On Vulkan, an integrated or software GPU counts as sharing memory. On OpenGL,
    /// only Mesa's drivers on Linux report it, through an X display.
    /// </remarks>
    public static bool? UnifiedMemory =>
        RetooledNative.RetooledNative_UnifiedMemory() switch
        {
            RetooledNative.QueryResult.Yes => true,
            RetooledNative.QueryResult.No => false,
            _ => null,
        };
}
