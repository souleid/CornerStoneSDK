#!/usr/bin/env python3
# Copyright 2025 souleid

import os
import sys
import shutil
import argparse
import subprocess
from pathlib import Path

NDK_DEFAULT = Path.home() / "Android" / "Sdk" / "ndk" / "26.2.11394342"
ABIS = ["arm64-v8a", "x86_64"]  # add "armeabi-v7a" if needed
MIN_SDK = "23"
BUILD_TYPE = "Release"

SCRIPT_DIR = Path(__file__).resolve().parent
CMAKE_DIR = SCRIPT_DIR  # this file sits next to CMakeLists for Unity
OUT_DIR = SCRIPT_DIR / "out"

def run(cmd, cwd=None):
    print("[CMD]", " ".join(cmd))
    subprocess.run(cmd, cwd=cwd, check=True)

def build_abi(abi: str, ndk: Path):
    bdir = OUT_DIR / abi
    bdir.mkdir(parents=True, exist_ok=True)
    toolchain = ndk / "build" / "cmake" / "android.toolchain.cmake"

    cfg = [
        "cmake",
        "-S", str(CMAKE_DIR),
        "-B", str(bdir),
        "-G", "Ninja",
        "-DCMAKE_SYSTEM_NAME=Android",
        f"-DCMAKE_SYSTEM_VERSION={MIN_SDK}",
        f"-DANDROID_PLATFORM=android-{MIN_SDK}",
        f"-DCMAKE_ANDROID_ARCH_ABI={abi}",
        f"-DANDROID_ABI={abi}",
        "-DCMAKE_ANDROID_STL_TYPE=c++_static",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
        f"-DCMAKE_BUILD_TYPE={BUILD_TYPE}",
    ]
    run(cfg)
    run(["cmake", "--build", str(bdir), "--config", BUILD_TYPE, "-j"])

    so = bdir / f"libCornerStoneSDK.so"
    if not so.exists():
        # Some generators put .so under bdir/<config>/
        alt = bdir / BUILD_TYPE / f"libCornerStoneSDK.so"
        if alt.exists():
            return alt
        raise FileNotFoundError(f".so not found for {abi}")
    return so

def copy_to_unity(so_path: Path, unity_root: Path, abi: str):
    dst_dir = (unity_root / "Assets" / "Plugins" / "Android" / abi)
    dst_dir.mkdir(parents=True, exist_ok=True)
    dst = dst_dir / so_path.name
    shutil.copy2(so_path, dst)
    print(f"[COPY] {so_path} -> {dst}")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ndk", default=os.environ.get("ANDROID_NDK", str(NDK_DEFAULT)))
    ap.add_argument("--abis", nargs="+", default=ABIS)
    ap.add_argument("--unity", default=None,
                    help="Unity project root to copy .so files")
    args = ap.parse_args()

    ndk = Path(args.ndk)
    if not ndk.exists():
        print(f"Error: ANDROID_NDK invalid: {ndk}")
        sys.exit(1)

    print("=== Build Android .so for Unity ===")
    print(f"NDK: {ndk}")

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    produced = []
    for abi in args.abis:
        print(f"\n=== {abi} ===")
        so = build_abi(abi, ndk)
        produced.append((abi, so))

    if args.unity:
        uroot = Path(args.unity).resolve()
        for abi, so in produced:
            copy_to_unity(so, uroot, abi)

    print("\n=== Done ===")
    for abi, so in produced:
        print(f"{abi}: {so}")

if __name__ == "__main__":
    main()
