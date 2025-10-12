// Copyright 2025 souleid
using System;
using System.Runtime.InteropServices;
using System.Threading;
using UnityEngine;
using CornerstoneNative;

namespace Cornerstone.Unity
{
  public sealed class CornerstoneComponent : MonoBehaviour
  {
    [Header("Native worker threads (0 = core default)")]
    [SerializeField] private uint numThreads = 0;

    private IntPtr _handle = IntPtr.Zero;

    private SynchronizationContext _ctx;

    private static CS.CSWorker s_workerTrampoline;
    private static CS.CSResultCallback s_doneTrampoline;

    private void Awake()
    {
      _ctx = SynchronizationContext.Current;

      var err = CS.CSCreate(numThreads, out _handle);
      if (err != CS.CS_OK || _handle == IntPtr.Zero)
      {
        Debug.LogError($"CSCreate failed ({err}): {CS.GetLastErrorString()}");
        _handle = IntPtr.Zero;
      }

      if (s_workerTrampoline == null) s_workerTrampoline = WorkerThunk;
      if (s_doneTrampoline == null) s_doneTrampoline = DoneThunk;
    }

    private void OnDestroy()
    {
      if (_handle != IntPtr.Zero)
      {
        CS.CSDestory(_handle);
        _handle = IntPtr.Zero;
      }
    }

    // ---------- Public API ----------

    /// <summary>
    /// Enqueue a unit of work. The 'worker' runs on a pool thread.
    /// 'onDone' is invoked on Unity main thread with (id, err).
    /// Returns submitted task id, or 0 on failure.
    /// </summary>
    public ulong Request(Action worker, Action<ulong, int> onDone)
    {
      if (_handle == IntPtr.Zero)
      {
        Debug.LogError("CornerstoneComponent not initialized.");
        return 0;
      }
      if (worker == null || onDone == null)
      {
        Debug.LogError("worker/onDone must not be null.");
        return 0;
      }

      // Pack worker action for the native worker trampoline.
      var workHandle = GCHandle.Alloc(worker, GCHandleType.Normal);

      // Pack callback ctx for the native completion trampoline.
      var cbCtx = new CallbackCtx { self = this, onDone = onDone };
      var cbHandle = GCHandle.Alloc(cbCtx, GCHandleType.Normal);

      ulong id;
      var err = CS.CSRequest(
        _handle,
        s_workerTrampoline,
        GCHandle.ToIntPtr(workHandle),
        s_doneTrampoline,
        GCHandle.ToIntPtr(cbHandle),
        out id
      );

      if (err != CS.CS_OK)
      {
        Debug.LogError($"CSRequest failed ({err}): {CS.GetLastErrorString()}");
        // Avoid leaks on failure.
        if (workHandle.IsAllocated) workHandle.Free();
        if (cbHandle.IsAllocated) cbHandle.Free();
        return 0;
      }

      return id;
    }

    /// <summary>
    /// Cancel a single task by id.
    /// </summary>
    public void Cancel(ulong id)
    {
      if (_handle == IntPtr.Zero) return;
      var e = CS.CSCancel(_handle, id);
      if (e != CS.CS_OK)
      {
        Debug.LogError($"CSCancel failed ({e}): {CS.GetLastErrorString()}");
      }
    }

    /// <summary>
    /// Cancel all queued tasks.
    /// </summary>
    public void CancelAll()
    {
      if (_handle == IntPtr.Zero) return;
      var e = CS.CSCancelAll(_handle);
      if (e != CS.CS_OK)
      {
        Debug.LogError($"CSCancelAll failed ({e}): {CS.GetLastErrorString()}");
      }
    }

    // ---------- Trampolines (called from native worker threads) ----------

    private sealed class CallbackCtx
    {
      public CornerstoneComponent self;
      public Action<ulong, int> onDone;
    }

    // Runs on a native pool thread. No Unity API allowed here.
    [AOT.MonoPInvokeCallback(typeof(CS.CSWorker))]
    private static void WorkerThunk(IntPtr workArg)
    {
      var h = GCHandle.FromIntPtr(workArg);
      try
      {
        var action = (Action)h.Target;
        action?.Invoke();
      }
      finally
      {
        // Release worker payload.
        if (h.IsAllocated) h.Free();
      }
    }

    // Runs on a native pool thread. Hop to main thread before touching Unity.
    [AOT.MonoPInvokeCallback(typeof(CS.CSResultCallback))]
    private static void DoneThunk(ulong id, int err, IntPtr user)
    {
      var h = GCHandle.FromIntPtr(user);
      CallbackCtx ctxObj = null;
      try
      {
        ctxObj = (CallbackCtx)h.Target;
      }
      finally
      {
        // Release callback payload.
        if (h.IsAllocated) h.Free();
      }

      if (ctxObj == null || ctxObj.self == null)
        return;

      var self = ctxObj.self;
      var cb = ctxObj.onDone;

      // Cache refs for the post closure.
      void InvokeOnMain()
      {
        try { cb?.Invoke(id, err); }
        catch (Exception ex)
        {
          Debug.LogException(ex);
        }
      }

      var sc = self._ctx;
      if (sc != null)
      {
        sc.Post(_ => InvokeOnMain(), null);
      }
      else
      {
        // Fallback if context is missing (very early lifecycle).
        // Avoid heavy Unity API here.
        InvokeOnMain();
      }
    }
  }
}
