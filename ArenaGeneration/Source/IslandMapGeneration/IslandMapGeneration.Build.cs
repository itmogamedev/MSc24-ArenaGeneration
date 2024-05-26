// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IslandMapGeneration : ModuleRules
{
	public IslandMapGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Voronoi",
			"ProceduralMeshComponent", "GeometryFramework",
			"GeometryCore",
			"DynamicMesh", "WaveFunctionCollapse"
		});
	}
}