#!/usr/bin/env python3
import os
import shutil
import sys

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(REPO_ROOT, "src")
UE_CORE = os.path.join(
    REPO_ROOT, "unreal", "CornerStoneUE", "Source", "ThirdParty", "CornerStoneCore"
)

# Copy-only list (relative to src/)
HEADER_FILES = [
    "cornerstonesdkcore.h",
    "memory/csmemory_api.h",
    "memory/csmemoryallocator.h",
    "memory/csnewdelete.h",
    "memory/memoryutils.h",
    "thread/csthread_api.h",
    "thread/csthreadallocator.h",
    "thread/managedthreadpool.h",
    "thread/managedthreadpool_impl.h",
]

# Input → actual folder name mapping
PLATFORM_MAP = {
    "Win64": "x64",
    "Win32": "x86",
    "Linux": "Linux",
    "Mac": "Mac",
    "Android": "Android",
    "iOS": "iOS",
    "PS5": "PS5",
    "XboxSeriesX": "XboxSeriesX",
    "Switch": "Switch",
}

# Which library file extensions to collect per (input) platform
# (static preferred: Windows/Xbox -> .lib, others -> .a)
PLATFORM_LIB_EXTS = {
    "Win64": [".lib"],
    "Win32": [".lib"],
    "Linux": [".a"],
    "Mac": [".a"],
    "Android": [".a"],
    "iOS": [".a"],
    "PS5": [".a"],          # SIE SDK 보통 정적은 .a
    "XboxSeriesX": [".lib"],
    "Switch": [".a"],
}

def copy_headers_from_src():
    """Copy only whitelisted headers from src/ with directory structure."""
    dst_inc_root = os.path.join(UE_CORE, "Include", "src")
    if os.path.exists(dst_inc_root):
        shutil.rmtree(dst_inc_root)
    os.makedirs(dst_inc_root, exist_ok=True)

    for rel in HEADER_FILES:
        src_file = os.path.join(SRC_DIR, rel)
        dst_file = os.path.join(dst_inc_root, rel)

        if not os.path.exists(src_file):
            print(f"[WARN] Missing header: {src_file}")
            continue

        os.makedirs(os.path.dirname(dst_file), exist_ok=True)
        shutil.copy2(src_file, dst_file)
        print(f"Copied {src_file} -> {dst_file}")

def copy_lib(platform: str):
    """Copy static libs from build/<mapped>/lib -> UE Lib/<mapped>."""
    mapped_folder = PLATFORM_MAP.get(platform, platform)
    build_lib_dir = os.path.join(REPO_ROOT, "build", mapped_folder, "lib")
    dst_lib_dir = os.path.join(UE_CORE, "Lib", mapped_folder)
    os.makedirs(dst_lib_dir, exist_ok=True)

    if not os.path.isdir(build_lib_dir):
        print(f"[WARN] build lib dir not found: {build_lib_dir}")
        return

    # Choose extensions for the *input* platform key
    exts = PLATFORM_LIB_EXTS.get(platform, [".lib", ".a"])
    copied = 0
    for fname in os.listdir(build_lib_dir):
        if any(fname.endswith(ext) for ext in exts):
            src = os.path.join(build_lib_dir, fname)
            shutil.copy2(src, dst_lib_dir)
            copied += 1
            print(f"Copied {src} -> {dst_lib_dir}")
    if copied == 0:
        print(f"[WARN] No libs with {exts} under {build_lib_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: copy_headers_and_libs.py <Platform>")
        print("Platforms:", ", ".join(PLATFORM_MAP.keys()))
        sys.exit(1)

    platform = sys.argv[1]
    copy_headers_from_src()
    copy_lib(platform)
    print(f"[OK] Headers and libs prepared for {platform}")
