// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGenerationTarget : TargetRules
{
	public ArenaGenerationTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("ArenaGeneration");
		ExtraModuleNames.Add("IslandMapGeneration");
		ExtraModuleNames.Add("SSHelper");
		ExtraModuleNames.Add("WaveFunctionCollapse");
	}
}
