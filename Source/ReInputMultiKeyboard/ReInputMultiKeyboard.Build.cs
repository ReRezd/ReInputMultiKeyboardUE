// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ReInputMultiKeyboard : ModuleRules
{
	public ReInputMultiKeyboard(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicSystemLibraries.AddRange(
			new string[] {
				"setupapi.lib",
				"hid.lib",
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"InputCore",
				"ApplicationCore",
				"InputDevice",
				"UMG",
				"Slate"
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
	}
}
