namespace Retooled.Native;

public static class SystemInfo
{
    /// <summary>
    /// Does the current graphics device use system memory?
    /// </summary>
    /// 
    /// <remarks>
    /// Not all graphics APIs or devices expose this. If this cannot be determined
    /// then this will be <c>null</c>.
    /// </remarks>
    public static bool? HasUnifiedMemory =>
        NativeAPI.Retooled_HasUnifiedMemory() switch
        {
            QueryResult.True => true,
            QueryResult.False => false,
            _ => null,
        };
}
