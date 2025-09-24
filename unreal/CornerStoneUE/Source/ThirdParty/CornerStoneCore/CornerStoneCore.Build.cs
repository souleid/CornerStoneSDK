// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class CornerStoneCore : ModuleRules
{
	public CornerStoneCore(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
        bEnableExceptions = true;

        string IncPath = Path.Combine(ModuleDirectory, "Include");
        PublicIncludePaths.Add(IncPath);

        string LibPath = Path.Combine(ModuleDirectory, "Lib");

        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(LibPath, "x64", "CornerStoneSDK.lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
            PublicAdditionalLibraries.Add(Path.Combine(LibPath, "Mac", "libCornerStoneCore.a"));
        }
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			 PublicAdditionalLibraries.Add(Path.Combine(LibPath, "Android", "arm64-v8a", "libcornerstone.a"));
        }
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			 PublicAdditionalLibraries.Add(Path.Combine(LibPath, "IOS", "libcornerstone.a"));
        }
	}
}
