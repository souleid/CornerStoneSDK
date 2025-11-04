# CornerStoneSDK

Lightweight, High-Performance Asynchronous SDK Framework for Cross-Platform Service Modules.

---

## Project Overview & Core Value

**CornerStoneSDK** is a **lightweight SDK framework** built to **research and implement** custom memory management and multithreaded environments. 

This project is designed to solve common problems when integrating services into performance-sensitive applications, like game engines.

**Primary Goal:** To efficiently **separate** service logic from the main application thread, and to provide a **stable, predictable asynchronous environment** using a **custom memory allocator** and a **dedicated thread pool**.

## Key Features

* **High-Performance Core:**
    * **Managed Threading:** Implements an **independent thread pool** to separate service logic from the main thread, **preventing host application responsiveness degradation**.
    * **Pluggable Memory Allocation Strategy:** The SDK is designed to use its **own optimized custom allocator**, but it also allows **injecting host application (engine) allocators**. This critical feature ensures **memory profiling and debugging consistency** across the entire application stack.
* **Asynchronous Architecture:**
    * **Non-Blocking Design:** All core functionalities operate **non-blockingly** (e.g., via futures/promises), guaranteeing high concurrency and minimal latency during service execution.
* **Lightweight & Extensible:**
    * **Minimal Dependencies:** Maintains low external library dependencies, making the SDK a **lightweight framework foundation** for simplified build and integration.
    * **Cross-Platform Ready:** Adheres strictly to C++ standards, ensuring **high portability** across various operating systems.
