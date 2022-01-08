// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataprepSelectionTransforms.h"
#include "DataprepCorePrivateUtils.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Texture.h"
#include "Materials/MaterialInstance.h"

#define LOCTEXT_NAMESPACE "DataprepSelectionTransforms"

namespace DataprepSelectionTransformsUtils
{
	template <typename T>
	TSet<T*> GetDataprepObjects()
	{
		TSet<T*> Objects;

		for (TObjectIterator<T> It; It; ++It)
		{
			if (const UPackage* Package = It->GetPackage())
			{
				const FString PackageName = Package->GetName();

				if (PackageName.StartsWith(DataprepCorePrivateUtils::GetRootPackagePath()))
				{
					Objects.Add(*It);
				}
			}
		}

		return MoveTemp(Objects);
	}
}

void UDataprepReferenceSelectionTransform::OnExecution_Implementation(const TArray<UObject*>& InObjects, TArray<UObject*>& OutObjects)
{
	TSet<UObject*> Assets;

	for (UObject* Object : InObjects)
	{
		if (!ensure(Object) || Object->IsPendingKill())
		{
			continue;
		}

		if (AActor* Actor = Cast< AActor >(Object))
		{
			TArray<UActorComponent*> Components = Actor->GetComponents().Array();
			Components.Append(Actor->GetInstanceComponents());

			for (UActorComponent* Component : Components)
			{
				if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Component))
				{
					if (UStaticMesh* StaticMesh = MeshComponent->GetStaticMesh())
					{
						Assets.Add(StaticMesh);
					}

					for (UMaterialInterface* MaterialInterface : MeshComponent->OverrideMaterials)
					{
						Assets.Add(MaterialInterface);
					}
				}
			}
		}
		else if (UStaticMesh* StaticMesh = Cast< UStaticMesh >(Object))
		{
			for (FStaticMaterial& StaticMaterial : StaticMesh->GetStaticMaterials())
			{
				if (UMaterialInterface* MaterialInterface = StaticMaterial.MaterialInterface)
				{
					Assets.Add(MaterialInterface);
				}
			}

			if (bOutputCanIncludeInput)
			{
				Assets.Add(Object);
			}
		}
		else if (UMaterialInterface* MaterialInterface = Cast< UMaterialInterface >(Object))
		{
			Assets.Add(MaterialInterface);

			if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(MaterialInterface))
			{
				if (MaterialInstance->Parent)
				{
					Assets.Add(MaterialInstance->Parent);
				}
			}

			// Collect textures
			TArray<UTexture*> Textures;
			MaterialInterface->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
			for (UTexture* Texture : Textures)
			{
				Assets.Add(Texture);
			}

			if (bOutputCanIncludeInput)
			{
				Assets.Add(Object);
			}
		}
	}

	OutObjects.Append(Assets.Array());
}

void UDataprepReferencedSelectionTransform::OnExecution_Implementation(const TArray<UObject*>& InObjects, TArray<UObject*>& OutObjects)
{
	TSet<UObject*> Assets;

	TFunction<bool(const UMaterialInterface*, const UTexture*)> DoesMaterialUseTexture = [](const UMaterialInterface* Material, const UTexture* CheckTexture) -> bool
	{
		TArray<UTexture*> Textures;
		Material->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
		for (int32 i = 0; i < Textures.Num(); i++)
		{
			if (Textures[i] == CheckTexture)
			{
				return true;
			}
		}
		return false;
	};

	TSet<AActor*> Actors = DataprepSelectionTransformsUtils::GetDataprepObjects<AActor>();
	TSet<UStaticMesh*> Meshes = DataprepSelectionTransformsUtils::GetDataprepObjects<UStaticMesh>();
	TSet<UMaterialInterface*> Materials = DataprepSelectionTransformsUtils::GetDataprepObjects<UMaterialInterface>();

	for (UObject* Object : InObjects)
	{
		if (!ensure(Object) || Object->IsPendingKill())
		{
			continue;
		}

		if (UStaticMesh* StaticMesh = Cast< UStaticMesh >(Object))
		{
			// Collect actors referencing this mesh
			for (AActor* Actor : Actors)
			{
				TInlineComponentArray<UStaticMeshComponent*> StaticMeshComponents(Actor);
				for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
				{
					UStaticMesh* ActorMesh = StaticMeshComponent->GetStaticMesh();
					if (ActorMesh == StaticMesh)
					{
						Assets.Add(Actor);
					}
				}
			}
		}
		else if (UMaterialInterface* MaterialInterface = Cast< UMaterialInterface >(Object))
		{
			// Collect actor components referencing this material (material overrides)
			for (AActor* Actor : Actors)
			{
				TInlineComponentArray<UMeshComponent*> MeshComponents(Actor);
				for (UMeshComponent* MeshComponent : MeshComponents)
				{
					for (UMaterialInterface* MeshComponentMaterialInterface : MeshComponent->OverrideMaterials)
					{
						if (MeshComponentMaterialInterface == MaterialInterface)
						{
							Assets.Add(Actor);
						}
					}
				}
			}

			// Collect meshes referencing this material
			for (UStaticMesh* Mesh : Meshes)
			{
				for (FStaticMaterial& StaticMaterial : Mesh->GetStaticMaterials())
				{
					UMaterialInterface* StaticMaterialInterface = StaticMaterial.MaterialInterface;
					if (StaticMaterialInterface == MaterialInterface)
					{
						Assets.Add(Mesh);
					}
				}
			}

			// Collect material instances referencing this material
			if (UMaterial* Material = Cast<UMaterial>(MaterialInterface))
			{
				for (UMaterialInterface* MatInterface : Materials)
				{
					if (MatInterface->GetMaterial() == Material)
					{
						Assets.Add(MatInterface);
					}
				}
			}
		}
		else if (UTexture* Texture = Cast< UTexture >(Object))
		{
			// Collect materials referencing this texture
			for (UMaterialInterface* MatInterface : Materials)
			{
				if (DoesMaterialUseTexture(MatInterface, Texture))
				{
					if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(MatInterface))
					{
						Assets.Add(MaterialInstance);
						if (MaterialInstance->Parent)
						{
							Assets.Add(MaterialInstance->Parent);
						}
					}
					else if (UMaterial* Material = Cast<UMaterial>(MatInterface))
					{
						Assets.Add(Material);
					}
				}
			}
		}
	}

	OutObjects.Append(Assets.Array());
}

void UDataprepHierarchySelectionTransform::OnExecution_Implementation(const TArray<UObject*>& InObjects, TArray<UObject*>& OutObjects)
{
	TArray<AActor*> ActorsToVisit;

	for (UObject* Object : InObjects)
	{
		if (!ensure(Object) || Object->IsPendingKill())
		{
			continue;
		}

		if (AActor* Actor = Cast< AActor >(Object))
		{
			TArray<AActor*> Children;
			Actor->GetAttachedActors( Children );

			ActorsToVisit.Append( Children );
		}
	}

	TSet<UObject*> NewSelection;

	while ( ActorsToVisit.Num() > 0)
	{
		AActor* VisitedActor = ActorsToVisit.Pop();
		if (VisitedActor == nullptr)
		{
			continue;
		}

		NewSelection.Add(VisitedActor);

		if(SelectionPolicy == EDataprepHierarchySelectionPolicy::AllDescendants)
		{
			// Continue with children
			TArray<AActor*> Children;
			VisitedActor->GetAttachedActors( Children );

			ActorsToVisit.Append( Children );
		}
	}

	OutObjects.Append(NewSelection.Array());

	if (bOutputCanIncludeInput)
	{
		OutObjects.Reserve( OutObjects.Num() + InObjects.Num());

		for (UObject* Object : InObjects)
		{
			if (!ensure(Object) || Object->IsPendingKill())
			{
				continue;
			}

			if (AActor* Actor = Cast< AActor >(Object))
			{
				OutObjects.Add(Object);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
