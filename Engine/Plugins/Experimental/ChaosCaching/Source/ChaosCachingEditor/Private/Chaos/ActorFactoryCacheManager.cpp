// Copyright Epic Games, Inc. All Rights Reserved.

#include "Chaos/ActorFactoryCacheManager.h"
#include "AssetData.h"
#include "Chaos/CacheCollection.h"
#include "Chaos/CacheManagerActor.h"
#include "Chaos/ChaosCache.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet2/ComponentEditorUtils.h"

#define LOCTEXT_NAMESPACE "CacheManagerActorFactory"

UActorFactoryCacheManager::UActorFactoryCacheManager()
{
	DisplayName            = LOCTEXT("DisplayName", "Chaos Cache Manager");
	NewActorClass          = AChaosCacheManager::StaticClass();
	bUseSurfaceOrientation = false;
}

bool UActorFactoryCacheManager::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if(!AssetData.IsValid() || !AssetData.GetClass()->IsChildOf(UChaosCacheCollection::StaticClass()))
	{
		OutErrorMsg = LOCTEXT("NoCollection", "A valid cache collection must be specified.");
		return false;
	}

	return true;
}

void UActorFactoryCacheManager::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	AChaosCacheManager*    Manager    = Cast<AChaosCacheManager>(NewActor);
	UChaosCacheCollection* Collection = Cast<UChaosCacheCollection>(Asset);
	
	// The cachemanager exists now, start adding our spawnables
	if (Manager && Collection)
	{
		if (!Manager->bIsEditorPreviewActor)
		{
			if (UWorld* World = Manager->GetWorld())
			{
				// Remove any pre-existing components
				Manager->ClearObservedComponents();
				
				Manager->CacheCollection = Collection;
				for (UChaosCache* Cache : Collection->GetCaches())
				{
					if (!Cache)
					{
						continue;
					}

					const FCacheSpawnableTemplate& Template = Cache->GetSpawnableTemplate();
					if (Template.DuplicatedTemplate)
					{
						check(Template.DuplicatedTemplate->GetClass()->IsChildOf(UPrimitiveComponent::StaticClass()));

						FObjectDuplicationParameters Parameters(Template.DuplicatedTemplate, Manager);
						Parameters.FlagMask &= ~RF_Transient;
						Parameters.FlagMask &= ~RF_DefaultSubObject;
						Parameters.FlagMask &= ~RF_Transactional;
						
						UPrimitiveComponent* NewComponent = CastChecked<UPrimitiveComponent>(StaticDuplicateObjectEx(Parameters));
						NewComponent->SetupAttachment(Manager->GetRootComponent());
						NewComponent->SetRelativeTransform(Template.ComponentTransform);
						Manager->AddInstanceComponent(NewComponent);
						NewComponent->RegisterComponent();

						FObservedComponent& Observed = Manager->AddNewObservedComponent(NewComponent);
						// AddNewObservedComponent will have given this a unique name as if it was going to build a new cache, override to the actual cache name
						Observed.CacheName = Cache->GetFName();
						Observed.CacheMode = ECacheMode::Play;
						Observed.StartMode = EStartMode::Timed;
						Observed.TimedDuration = 0.0f;
					}
				}
			}
		}
	}
}

UObject* UActorFactoryCacheManager::GetAssetFromActorInstance(AActor* ActorInstance)
{
	if(AChaosCacheManager* Manager = Cast<AChaosCacheManager>(ActorInstance))
	{
		return Manager->CacheCollection;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
