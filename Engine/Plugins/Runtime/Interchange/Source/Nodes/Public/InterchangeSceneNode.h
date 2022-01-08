// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/InterchangeBaseNode.h"

#include "InterchangeSceneNode.generated.h"


UCLASS(BlueprintType)
class INTERCHANGENODES_API UInterchangeSceneNode : public UInterchangeBaseNode
{
	GENERATED_BODY()

public:
	UInterchangeSceneNode()
	:UInterchangeBaseNode()
	{
	}

	/**
	 * Return the node type name of the class, we use this when reporting error
	 */
	virtual FString GetTypeName() const override
	{
		const FString TypeName = TEXT("SceneNode");
		return TypeName;
	}

	virtual FGuid GetHash() const override
	{
		return Attributes->GetStorageHash();
	}

	/** Return false if the Attribute was not set previously.*/
	UFUNCTION(BlueprintCallable, Category = "Interchange | Node | Scene")
	bool GetCustomLocalTransform(FTransform& AttributeValue) const
	{
		IMPLEMENT_NODE_ATTRIBUTE_GETTER(LocalTransform, FTransform);
	}

	UFUNCTION(BlueprintCallable, Category = "Interchange | Node | Scene")
	bool SetCustomLocalTransform(const FTransform& AttributeValue)
	{
		IMPLEMENT_NODE_ATTRIBUTE_SETTER_NODELEGATE(LocalTransform, FTransform);
	}

	/** Return false if the Attribute was not set previously.*/
	UFUNCTION(BlueprintCallable, Category = "Interchange | Node | Scene")
	bool GetCustomGlobalTransform(FTransform& AttributeValue) const
	{
		IMPLEMENT_NODE_ATTRIBUTE_GETTER(GlobalTransform, FTransform);
	}

	UFUNCTION(BlueprintCallable, Category = "Interchange | Node | Scene")
	bool SetCustomGlobalTransform(const FTransform& AttributeValue)
	{
		IMPLEMENT_NODE_ATTRIBUTE_SETTER_NODELEGATE(GlobalTransform, FTransform);
	}

private:
	//Scene Attribute Keys
	const UE::Interchange::FAttributeKey Macro_CustomLocalTransformKey = UE::Interchange::FAttributeKey(TEXT("LocalTransform"));
	const UE::Interchange::FAttributeKey Macro_CustomGlobalTransformKey = UE::Interchange::FAttributeKey(TEXT("GlobalTransform"));
};
