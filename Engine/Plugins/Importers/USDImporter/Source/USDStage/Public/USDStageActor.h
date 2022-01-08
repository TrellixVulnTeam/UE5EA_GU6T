// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Misc/SecureHash.h"
#include "Templates/Function.h"

#include "UnrealUSDWrapper.h"
#include "USDAssetCache.h"
#include "USDLevelSequenceHelper.h"
#include "USDListener.h"
#include "USDMemory.h"
#include "USDPrimTwin.h"
#include "USDSkeletalDataConversion.h"

#include "UsdWrappers/UsdStage.h"
#include "UsdWrappers/UsdPrim.h"
#include "UsdWrappers/SdfPath.h"

#include "USDStageActor.generated.h"

class ALevelSequenceActor;
class IMeshBuilderModule;
class ULevelSequence;
class UMaterial;
class UUsdAsset;
enum class EMapChangeType : uint8;
enum class EUsdPurpose : int32;
struct FMeshDescription;
struct FUsdSchemaTranslationContext;

UCLASS( MinimalAPI )
class AUsdStageActor : public AActor
{
	GENERATED_BODY()

	friend struct FUsdStageActorImpl;
	friend class FUsdLevelSequenceHelperImpl;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USD", meta = (RelativeToGameDir))
	FFilePath RootLayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USD")
	EUsdInitialLoadSet InitialLoadSet;

	/* Only load prims with these specific purposes from the USD file */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USD", meta = (Bitmask, BitmaskEnum=EUsdPurpose))
	int32 PurposesToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USD")
	FName RenderContext;

public:
	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API void SetRootLayer(const FString& RootFilePath );

	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API void SetInitialLoadSet( EUsdInitialLoadSet NewLoadSet );

	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API void SetPurposesToLoad( int32 NewPurposesToLoad );

	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API void SetRenderContext( const FName& NewRenderContext );

	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API float GetTime() const { return Time; }

	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API void SetTime(float InTime);

	/**
	 * Gets the transient component that was generated for a prim with a given prim path.
	 * Warning: The lifetime of the component is managed by the AUsdStageActor, and it may be force-destroyed at any time (e.g. when closing the stage)
	 * @param PrimPath - Full path to the source prim, e.g. "/root_prim/my_prim"
	 * @return The corresponding spawned component. It may correspond to a parent prim, if the prim at PrimPath was collapsed. Nullptr if path is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API USceneComponent* GetGeneratedComponent( const FString& PrimPath );

	/**
	 * Gets the transient assets that were generated for a prim with a given prim path. Likely one asset (e.g. UStaticMesh), but can be multiple (USkeletalMesh, USkeleton, etc.)
	 * @param PrimPath - Full path to the source prim, e.g. "/root_prim/my_mesh"
	 * @return The corresponding generated assets. May be empty if path is invalid or if that prim led to no generated assets.
	 */
	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API TArray<UObject*> GetGeneratedAssets( const FString& PrimPath );

	/**
	 * Gets the path to the prim that was parsed to generate the given `Object`.
	 * @param Object - UObject to query with. Can be one of the transient components generated when a stage was opened, or something like a UStaticMesh.
	 * @return The path to the source prim, e.g. "/root_prim/some_prim". May be empty in case we couldn't find the source prim.
	 */
	UFUNCTION(BlueprintCallable, Category = "USD", meta = (CallInEditor = "true"))
	USDSTAGE_API FString GetSourcePrimPath( UObject* Object );

private:
	UPROPERTY(Category = UsdStageActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	/* TimeCode to evaluate the USD stage at */
	UPROPERTY(EditAnywhere, Category = "USD")
	float Time;

	UPROPERTY()
	float StartTimeCode_DEPRECATED;

	UPROPERTY()
	float EndTimeCode_DEPRECATED;

	UPROPERTY()
	float TimeCodesPerSecond_DEPRECATED;

	UPROPERTY(VisibleAnywhere, Category = "USD", Transient)
	ULevelSequence* LevelSequence;

public:
	DECLARE_EVENT_OneParam( AUsdStageActor, FOnActorLoaded, AUsdStageActor* );
	USDSTAGE_API static FOnActorLoaded OnActorLoaded;

	DECLARE_EVENT( AUsdStageActor, FOnStageActorEvent );
	FOnStageActorEvent OnStageChanged;
	FOnStageActorEvent OnActorDestroyed;

	DECLARE_EVENT_TwoParams( AUsdStageActor, FOnPrimChanged, const FString&, bool );
	FOnPrimChanged OnPrimChanged;

	DECLARE_MULTICAST_DELEGATE(FOnUsdStageTimeChanged);
	FOnUsdStageTimeChanged OnTimeChanged;

public:
	AUsdStageActor();
	virtual ~AUsdStageActor();

	USDSTAGE_API void Reset() override;
	void Refresh() const;
	void ReloadAnimations();
	float GetTime() { return Time; }
	UUsdAssetCache* GetAssetCache() { return AssetCache; }
	TMap< FString, TMap< FString, int32 > > GetMaterialToPrimvarToUVIndex() { return MaterialToPrimvarToUVIndex; }

public:
#if WITH_EDITOR
	virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;
#endif // WITH_EDITOR
	virtual void PostDuplicate( bool bDuplicateForPIE ) override;
	virtual void PostLoad() override;
	virtual void Serialize(FArchive& Ar) override;
	virtual void Destroyed() override;

	void OnLevelAddedToWorld(ULevel* Level, UWorld* World);
	void OnLevelRemovedFromWorld(ULevel* Level, UWorld* World);

	void OnPreUsdImport( FString FilePath );
	void OnPostUsdImport( FString FilePath );

private:
	void OpenUsdStage();
	void LoadUsdStage();

#if WITH_EDITOR
	void OnMapChanged(UWorld* World, EMapChangeType ChangeType);
	void OnBeginPIE(bool bIsSimulating);
	void OnPostPIEStarted(bool bIsSimulating);
#endif // WITH_EDITOR

	void UpdateSpawnedObjectsTransientFlag( bool bTransient );

	void OnPrimsChanged( const TMap< FString, bool >& PrimsChangedList );
	void OnUsdPrimTwinDestroyed( const UUsdPrimTwin& UsdPrimTwin );

	void OnObjectPropertyChanged( UObject* ObjectBeingModified, FPropertyChangedEvent& PropertyChangedEvent );
	void HandlePropertyChangedEvent( FPropertyChangedEvent& PropertyChangedEvent );
	bool HasAuthorityOverStage() const;

private:
	UPROPERTY(Transient)
	UUsdPrimTwin* RootUsdTwin;

	UPROPERTY(Transient)
	TSet< FString > PrimsToAnimate;

	UPROPERTY(Transient)
	TMap< UObject*, FString > ObjectsToWatch;

	UPROPERTY(VisibleAnywhere, Category = "USD", AdvancedDisplay)
	UUsdAssetCache* AssetCache;

private:
	/** Keep track of blend shapes so that we can map 'inbetween shapes' to their separate morph targets when animating */
	UsdUtils::FBlendShapeMap BlendShapesByPath;

	/**
	 * When parsing materials, we keep track of which primvar we mapped to which UV channel.
	 * When parsing meshes later, we use this data to place the correct primvar values in each UV channel.
	 * We keep this here as these are generated when the materials stored in AssetsCache are parsed, so it should accompany them
	 */
	TMap< FString, TMap< FString, int32 > > MaterialToPrimvarToUVIndex;

public:
	USDSTAGE_API UE::FUsdStage& GetUsdStage();
	USDSTAGE_API const UE::FUsdStage& GetUsdStage() const;

	FUsdListener& GetUsdListener() { return UsdListener; }
	const FUsdListener& GetUsdListener() const { return UsdListener; }

	/** Prevents writing back data to the USD stage whenever our LevelSequences are modified */
	USDSTAGE_API void StopMonitoringLevelSequence();
	USDSTAGE_API void ResumeMonitoringLevelSequence();

	UUsdPrimTwin* GetOrCreatePrimTwin( const UE::FSdfPath& UsdPrimPath );
	UUsdPrimTwin* ExpandPrim( const UE::FUsdPrim& Prim, FUsdSchemaTranslationContext& TranslationContext );
	void UpdatePrim( const UE::FSdfPath& UsdPrimPath, bool bResync, FUsdSchemaTranslationContext& TranslationContext );

protected:
	/** Loads the asset for a single prim */
	void LoadAsset( FUsdSchemaTranslationContext& TranslationContext, const UE::FUsdPrim& Prim );

	/** Loads the assets for all prims from StartPrim and its children */
	void LoadAssets( FUsdSchemaTranslationContext& TranslationContext, const UE::FUsdPrim& StartPrim );

	void AnimatePrims();

private:
	UE::FUsdStage UsdStage;
	FUsdListener UsdListener;

	FUsdLevelSequenceHelper LevelSequenceHelper;

	FDelegateHandle OnRedoHandle;
};
