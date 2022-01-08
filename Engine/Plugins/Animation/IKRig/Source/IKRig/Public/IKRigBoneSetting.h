// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "IKRigBoneSetting.generated.h"

UCLASS(abstract, config = Engine, BlueprintType)
class IKRIG_API UIKRigBoneSetting : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Bone)
	FName Bone;
};

