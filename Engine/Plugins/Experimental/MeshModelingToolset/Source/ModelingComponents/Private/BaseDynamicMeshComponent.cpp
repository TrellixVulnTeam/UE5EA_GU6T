// Copyright Epic Games, Inc. All Rights Reserved. 

#include "BaseDynamicMeshComponent.h"



UBaseDynamicMeshComponent::UBaseDynamicMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}



void UBaseDynamicMeshComponent::SetOverrideRenderMaterial(UMaterialInterface* Material)
{
	if (OverrideRenderMaterial != Material)
	{
		OverrideRenderMaterial = Material;
		NotifyMaterialSetUpdated();
	}
}

void UBaseDynamicMeshComponent::ClearOverrideRenderMaterial()
{
	if (OverrideRenderMaterial != nullptr)
	{
		OverrideRenderMaterial = nullptr;
		NotifyMaterialSetUpdated();
	}
}





void UBaseDynamicMeshComponent::SetSecondaryRenderMaterial(UMaterialInterface* Material)
{
	if (SecondaryRenderMaterial != Material)
	{
		SecondaryRenderMaterial = Material;
		NotifyMaterialSetUpdated();
	}
}

void UBaseDynamicMeshComponent::ClearSecondaryRenderMaterial()
{
	if (SecondaryRenderMaterial != nullptr)
	{
		SecondaryRenderMaterial = nullptr;
		NotifyMaterialSetUpdated();
	}
}



void UBaseDynamicMeshComponent::SetSecondaryBuffersVisibility(bool bSecondaryVisibility)
{
	bDrawSecondaryBuffers = bSecondaryVisibility;
}

bool UBaseDynamicMeshComponent::GetSecondaryBuffersVisibility() const
{
	return bDrawSecondaryBuffers;
}




int32 UBaseDynamicMeshComponent::GetNumMaterials() const
{
	return BaseMaterials.Num();
}

UMaterialInterface* UBaseDynamicMeshComponent::GetMaterial(int32 ElementIndex) const 
{
	return (ElementIndex >= 0 && ElementIndex < BaseMaterials.Num()) ? BaseMaterials[ElementIndex] : nullptr;
}

FMaterialRelevance UBaseDynamicMeshComponent::GetMaterialRelevance(ERHIFeatureLevel::Type InFeatureLevel) const
{
	FMaterialRelevance Result = UMeshComponent::GetMaterialRelevance(InFeatureLevel);
	if (OverrideRenderMaterial)
	{
		Result |= OverrideRenderMaterial->GetRelevance_Concurrent(InFeatureLevel);
	}
	if (SecondaryRenderMaterial)
	{
		Result |= SecondaryRenderMaterial->GetRelevance_Concurrent(InFeatureLevel);
	}
	return Result;
}

void UBaseDynamicMeshComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	check(ElementIndex >= 0);
	if (ElementIndex >= BaseMaterials.Num())
	{
		BaseMaterials.SetNum(ElementIndex + 1, false);
	}
	BaseMaterials[ElementIndex] = Material;
}


void UBaseDynamicMeshComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	UMeshComponent::GetUsedMaterials(OutMaterials, bGetDebugMaterials);
	if (OverrideRenderMaterial != nullptr)
	{
		OutMaterials.Add(OverrideRenderMaterial);
	}
	if (SecondaryRenderMaterial != nullptr)
	{
		OutMaterials.Add(SecondaryRenderMaterial);
	}
}

