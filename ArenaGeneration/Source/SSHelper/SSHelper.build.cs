using UnrealBuildTool;

public class SSHelper : ModuleRules
{
	public SSHelper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });

		PublicIncludePaths.AddRange(new[] { "SSHelper/Public" });
		PrivateIncludePaths.AddRange(new[] { "SSHelper/Private" });
	}
}