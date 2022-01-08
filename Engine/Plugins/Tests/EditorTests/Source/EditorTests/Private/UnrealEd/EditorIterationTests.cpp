// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Misc/AutomationTest.h"
#include "Misc/App.h"
#include "UObject/Object.h"

#include "Tests/AutomationTestSettings.h"
#include "Tests/AutomationEditorCommon.h"
#include "Tests/AutomationCommon.h"
#include "AutomationControllerSettings.h"

#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"

class FFIterationOpenAssetsBase : public FAutomationTestBase
{
public:
	FFIterationOpenAssetsBase(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask)
	{
		const UAutomationControllerSettings* Settings = GetDefault<UAutomationControllerSettings>();
		bSuppressLogErrors = Settings->bSuppressLogErrors || !Settings->bTreatLogErrorsAsTestErrors;
		bElevateLogWarningsToErrors = Settings->bTreatLogWarningsAsTestErrors;
	}

	virtual bool SuppressLogErrors() override { return bSuppressLogErrors; }

	virtual bool ElevateLogWarningsToErrors() override { return bElevateLogWarningsToErrors; }

private:
	bool bSuppressLogErrors = false;
	bool bElevateLogWarningsToErrors = true;

};

/**
 * Test to open the sub editor windows for a specified list of assets.
 * This list can be setup in the Editor Preferences window within the editor or the DefaultEngine.ini file for that particular project.
*/
IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FIterationOpenAssets, FFIterationOpenAssetsBase, "Project.Iteration.OpenAssets", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

void FIterationOpenAssets::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	const UAutomationTestSettings* AutomationTestSettings = GetDefault<UAutomationTestSettings>();
	for ( FSoftObjectPath AssetRef : AutomationTestSettings->AssetsToOpen )
	{
		OutBeautifiedNames.Add(AssetRef.GetAssetName());
		OutTestCommands.Add(AssetRef.GetLongPackageName());
	}
}

bool FIterationOpenAssets::RunTest(const FString& LongAssetPath)
{
	//start with all editors closed
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllAssetEditors();

	// below is all latent action, so before sending there, verify the asset exists
	UObject* Object = FSoftObjectPath(LongAssetPath).TryLoad();
	if ( Object == nullptr )
	{
		UE_LOG(LogEditorAutomationTests, Error, TEXT("Failed to Open Asset '%s'."), *LongAssetPath);
		return false;
	}

	AddCommand(new FOpenEditorForAssetCommand(LongAssetPath));
	AddCommand(new FWaitLatentCommand(0.5f));
	AddCommand(new FDelayedFunctionLatentCommand([=] {
		if ( Object->GetOutermost()->IsDirty() )
		{
			UE_LOG(LogEditorAutomationTests, Error, TEXT("Asset '%s' was dirty after opening it."), *LongAssetPath);
		}
	}));
	AddCommand(new FCloseAllAssetEditorsCommand());
	AddCommand(new FDelayedFunctionLatentCommand([] {
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}));

	return true;
}
