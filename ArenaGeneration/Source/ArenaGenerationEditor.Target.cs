// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGenerationEditorTarget : TargetRules
{
	public ArenaGenerationEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("ArenaGeneration");
		ExtraModuleNames.Add("WaveFunctionCollapse");
		ExtraModuleNames.Add("SSHelper");
		ExtraModuleNames.Add("IslandMapGeneration");
	}
}
