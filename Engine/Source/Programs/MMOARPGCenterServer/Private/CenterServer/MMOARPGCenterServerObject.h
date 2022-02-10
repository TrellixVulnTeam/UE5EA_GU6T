#pragma once

#include "CoreMinimal.h"

// Plugins
#include "UObject/SimpleController.h"

#include "MMOARPGCenterServerObject.generated.h"

class USimpleMysqlObject;

UCLASS()
class UMMOARPGCenterServerObject : public USimpleController
{
	GENERATED_BODY()

public:
	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();
	virtual void RecvProtocol(uint32 InProtocol);
};
