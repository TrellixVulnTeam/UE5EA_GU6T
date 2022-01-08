// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class CoreTechExtension : ModuleRules
	{
		public CoreTechExtension(ReadOnlyTargetRules Target) : base(Target)
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
					"DataprepCore",
					"DatasmithContent",
					"CoreTechSurface",
					"DatasmithImporter",
					"Engine",
					"StaticMeshEditor",
					"Slate",
					"SlateCore",
					"StaticMeshDescription",
					"EditorFramework",
					"UnrealEd",
					"CADLibrary",
				}
			);
		}
	}
}
