// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "InterchangeSourceData.h"
#include "Mesh/InterchangeSkeletalMeshPayload.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "InterchangeSkeletalMeshPayloadInterface.generated.h"

UINTERFACE()
class INTERCHANGEIMPORT_API UInterchangeSkeletalMeshPayloadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Skeletal mesh payload interface. Derive from this interface if your payload can import skeletal mesh
 */
class INTERCHANGEIMPORT_API IInterchangeSkeletalMeshPayloadInterface
{
	GENERATED_BODY()
public:

	/**
	 * Once the translation is done, the import process need a way to retrieve payload data.
	 * This payload will be use by the factories to create the asset.
	 *
	 * @param PayloadKey - The key to retrieve the a particular payload contain into the specified source data.
	 * @return a PayloadData containing the data point by the payload key. The TOptional will not be set if there is an error.
	 */
	virtual TOptional<UE::Interchange::FSkeletalMeshLodPayloadData> GetSkeletalMeshLodPayloadData(const FString& PayloadKey) const = 0;
};


