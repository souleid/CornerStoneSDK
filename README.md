# CornerStoneSDK

> Cross-engine, multi-platform plugin framework — one pure C++14 core,
> injectable memory/thread allocators, thin bindings for Unreal & Unity.

![C++14](https://img.shields.io/badge/C%2B%2B-14-00599C?logo=cplusplus&logoColor=white)
![Unreal](https://img.shields.io/badge/Unreal_Engine-5-0E1128?logo=unrealengine&logoColor=white)
![Unity](https://img.shields.io/badge/Unity-6-000000?logo=unity&logoColor=white)
![Platforms](https://img.shields.io/badge/Win64_·_macOS_·_Android_·_iOS-444444)
![License](https://img.shields.io/badge/License-MIT-green)

## Why

Shipping a platform SDK into live game engines taught me two hard lessons:

1. **Every plugin project starts by rebuilding the same scaffolding** —
   threading, memory management, engine bindings, build scripts. Again and again.
2. **An SDK that allocates with `std::malloc` inside an engine that uses its own
   allocator is a crash waiting to happen.** I debugged exactly this in production:
   memory handed across the boundary between Unreal's binned malloc and the C++
   standard allocator, ending in fragmentation and crashes.

CornerStoneSDK is my answer to both: an engine-agnostic C++ core whose memory and
threading are **injected by the host**, plus thin per-engine bindings — so the same
core runs identically inside Unreal, Unity, or a bare console app.

## Architecture

Four layers, strictly separated — the core never includes an engine header:

```
┌───────────────────────────────────────────────────────────┐
│  Interface   Blueprint nodes (UE) · C# methods (Unity)    │
├───────────────────────────────────────────────────────────┤
│  Bridge      UObject wrapper · P/Invoke · marshalling     │
├───────────────────────────────────────────────────────────┤
│  Feature     download task · (OAuth, parser — planned)    │
├───────────────────────────────────────────────────────────┤
│  Core        adaptive memory allocator · thread pool      │
└───────────────────────────────────────────────────────────┘
```

### Adaptive memory allocator (injection)

The host passes its `malloc / realloc / free` at `Init()`. Every allocation the SDK
makes flows through the engine's allocator — no allocator mismatch across the DLL
boundary, and the SDK shows up in engine-level memory profiling.

```cpp
cornerstone::memory::MemoryAllocator host_alloc;
host_alloc.malloc_fn  = &EngineMalloc;   // e.g. FMemory::Malloc
host_alloc.realloc_fn = &EngineRealloc;
host_alloc.free_fn    = &EngineFree;

cornerstone::CornerstoneSdkCore sdk;
sdk.Init(/*num_threads=*/4, &host_alloc);
```

### Managed thread pool

Producer-consumer task queue with cancellation and result callbacks —
heavy work (large downloads, async requests) never hitches the main thread.

```cpp
auto task = cornerstone::util::MakeUnique<cornerstone::TaskDownload>(
    task_id, url, target_path, &OnProgress);
sdk.RequestTask(std::move(task), &OnResult);   // runs on the pool
sdk.CancelById(id);                            // cancels the task if still queued
```

> Cancellation is queue-level: a task that has already been picked up by a
> worker runs to completion. Cooperative cancellation for running tasks is on
> the roadmap.

▶ **Run it yourself:** [`samples/console_download`](samples/console_download/) —
downloads with live progress, cancellation, and a counting host allocator that
shows SDK-owned allocations routed through the injected functions.

> Scope note: objects the SDK owns explicitly (`CSNew` / `CSDelete`) go through
> the injected allocator. Internal STL containers, `std::string`, and libcurl's
> own allocations still use global `operator new` / `malloc`.

## Building

| Target | How |
|--------|-----|
| Windows (console/lib) | `v143/CornerStoneSDK.sln` (VS2022) |
| Android (.a / Unity .so) | `android/build_android.py` (CMake/NDK) |
| iOS | `ios/CornerStoneSDK.xcodeproj` |
| Unreal plugin | `unreal/CornerStoneUE` (`.uplugin`, prebuilt libs linked via ThirdParty module) |
| Unity | `unity/CornerStoneSDK` (C# runtime + native bindings) |

Dependencies: libcurl (bundled under `external/`). Style: Google C++ (`cpplint`).

## Status / Roadmap

- [x] Core — allocator injection, managed thread pool, task manager
- [x] Async download task (libcurl) with progress reporting
- [x] Unreal 5 plugin & Unity 6 native binding — verified on Win64 · macOS · Android · iOS
- [ ] Cooperative cancellation for running tasks — `Task::Cancel()` exists but is
      not yet wired to the task registry; only queued tasks can be cancelled today
- [ ] Alignment honoured in the default allocator — the `align` argument is
      passed through the API but dropped by the default `malloc` backend
- [ ] Exception-free core — `CSNew` throws `std::bad_alloc`, which does not fit
      engines built without exceptions
- [ ] OAuth task (iOS/Android)
- [ ] Allocator micro-benchmark (vs default malloc)
- [ ] CI (GitHub Actions: build + cpplint) & packaged releases (`.uplugin` / `.unitypackage`)

## Author

**Eunil Park (박은일)** — Game Client & SDK Engineer (C++, 12 yrs)
[Portfolio](https://souleid.github.io) · [LinkedIn](https://www.linkedin.com/in/eunil-park-kor) · souleid@gmail.com

Licensed under the [MIT License](LICENSE).
