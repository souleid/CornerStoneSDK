#!/usr/bin/env python3
# Copyright 2025 souleid
# Python build script for CornerStoneSDK static library (.a) for Android

import os
import sys
import subprocess
from pathlib import Path

# === Config ===
NDK_PATH = os.environ.get(
    "ANDROID_NDK", str(Path.home() / "Android" / "Sdk" / "ndk" / "25.2.9519653")
)
MIN_SDK = "23"                 # -> android-23
ABIS = ["arm64-v8a", "x86_64"]           # 필요하면 "armeabi-v7a" 추가
BUILD_TYPE = "Release"
SCRIPT_DIR = Path(__file__).resolve().parent
OUT_DIR = SCRIPT_DIR / "out"

TOOLCHAIN_FILE = Path(NDK_PATH) / "build" / "cmake" / "android.toolchain.cmake"


def run_cmd(cmd, cwd=None):
    print(f"[CMD] {' '.join(cmd)}")
    subprocess.run(cmd, cwd=cwd, check=True)


def build_for_abi(abi: str):
    build_dir = OUT_DIR / abi
    # 새 설정 반영 위해 캐시 없는 깨끗한 빌드를 권장
    if build_dir.exists():
        # 필요시 주석 해제해서 완전 삭제
        # import shutil; shutil.rmtree(build_dir)
        pass
    build_dir.mkdir(parents=True, exist_ok=True)

    cmake_configure = [
        "cmake",
        "-S",
        str(SCRIPT_DIR),
        "-B",
        str(build_dir),
        "-G", "Ninja",
        "-DCMAKE_SYSTEM_NAME=Android",
        f"-DCMAKE_SYSTEM_VERSION={MIN_SDK}",   # API level
        f"-DANDROID_PLATFORM=android-{MIN_SDK}",  # 일부 환경 호환
        f"-DCMAKE_ANDROID_ARCH_ABI={abi}",
        f"-DANDROID_ABI={abi}",
        "-DCMAKE_ANDROID_STL_TYPE=c++_static",
        f"-DCMAKE_TOOLCHAIN_FILE={TOOLCHAIN_FILE}",
        f"-DCMAKE_BUILD_TYPE={BUILD_TYPE}",
    ]

    run_cmd(cmake_configure)
    run_cmd(["cmake", "--build", str(build_dir), "--config", BUILD_TYPE, "-j"])


def main():
    if not Path(NDK_PATH).exists():
        print(f"Error: ANDROID_NDK not set or invalid path ({NDK_PATH})")
        sys.exit(1)

    print("=== CornerStoneSDK Android Build ===")
    print(f"Using NDK: {NDK_PATH}")

    for abi in ABIS:
        print(f"\n=== Building for {abi} ===")
        build_for_abi(abi)

    print("\n=== Build finished ===")
    print(f"Output: {OUT_DIR}/<ABI>/libcornerstone.a")


if __name__ == "__main__":
    main()
