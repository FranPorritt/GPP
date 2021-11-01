// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Final_Project : ModuleRules
{
	public Final_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
		});
	}
}
