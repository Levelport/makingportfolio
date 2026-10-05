// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSatCPP : ModuleRules
{
	public FPSatCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"FPSatCPP",
			"FPSatCPP/Variant_Horror",
			"FPSatCPP/Variant_Horror/UI",
			"FPSatCPP/Variant_Shooter",
			"FPSatCPP/Variant_Shooter/AI",
			"FPSatCPP/Variant_Shooter/UI",
			"FPSatCPP/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
