using System.Runtime.InteropServices;

namespace Retooled.Native;

/// <summary>Declarations for the RetooledNative plugin, whose source is in <c>Native~</c>.</summary>
internal static class RetooledNative
{
    private const string Plugin = "RetooledNative";

    /// <summary>Gpu.h's <c>Answer</c>.</summary>
    internal enum Answer
    {
        Unknown = -1,
        No = 0,
        Yes = 1,
    }

    [DllImport(Plugin)]
    internal static extern Answer RetooledNative_UnifiedMemory();
}
