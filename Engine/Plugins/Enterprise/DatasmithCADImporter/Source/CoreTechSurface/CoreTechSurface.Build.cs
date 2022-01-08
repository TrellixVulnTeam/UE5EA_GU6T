// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class CoreTechSurface : ModuleRules
	{
		public CoreTechSurface(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"MeshDescription",
					"DatasmithContent",
					"DatasmithTranslator",
					"Engine",
					"StaticMeshDescription",
					"CADLibrary",
				}
			);
		}
	}
}
