using System.Runtime.InteropServices;

namespace Retooled.Native;

internal enum QueryResult
{
    Unknown = -1,
    False = 0,
    True = 1,
}

internal static class NativeAPI
{
    private const string Plugin = "RetooledNative";


    [DllImport(Plugin)]
    internal static extern QueryResult Retooled_HasUnifiedMemory();
}
