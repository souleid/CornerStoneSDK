// Copyright 2025 souleid
using System;
using System.Runtime.InteropServices;

namespace Cornerstone.Native
{
  internal static class CS
  {
    private const string Dll =
#if UNITY_IOS
      "__Internal"
#else
      "CornerStoneSDK"
 /*
#elif UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
      "CornerStoneSDK"
#elif UNITY_STANDALONE_OSX
      "CornerStoneSDK"
  #elif UNITY_ANDROID
      "CornerStoneSDK"
*/
#endif
    public const int CS_OK = 0;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void CSWorker(IntPtr workArg);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void CSResultCallback(ulong id, int err, IntPtr user);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSCreate")]
    public static extern int CSCreate(uint numThreads, out IntPtr handle);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSDestory")]
    public static extern int CSDestory(IntPtr handle);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSRequest")]
    public static extern int CSRequest(
      IntPtr handle,
      CSWorker work,
      IntPtr workArg,
      CSResultCallback cb,
      IntPtr cbArg,
      out ulong outId
    );

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSCancel")]
    public static extern int CSCancel(IntPtr handle, ulong id);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSCancelAll")]
    public static extern int CSCancelAll(IntPtr handle);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl,
      EntryPoint = "CSLastError")]
    public static extern IntPtr CSLastError();

    public static string GetLastErrorString()
    {
      var p = CSLastError();
      return p == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(p);
    }
  }
}
