using UnrealBuildTool;

public class WaveFunctionCollapse : ModuleRules
{
	public WaveFunctionCollapse(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[] { "SSHelper" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayTags" });

		PublicIncludePaths.AddRange(new[] { "WaveFunctionCollapse/Public" });
		PrivateIncludePaths.AddRange(new[] { "WaveFunctionCollapse/Private" });
	}
}