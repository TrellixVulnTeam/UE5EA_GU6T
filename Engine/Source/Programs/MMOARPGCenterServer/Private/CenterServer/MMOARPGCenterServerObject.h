#pragma once

#include "CoreMinimal.h"

// Plugins
#include "UObject/SimpleController.h"
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

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

	void AddRegisterInfo(const FMMOARPGPlayerRegisterInfo& InRegisterInfo);

private:
	TMap<int32, FMMOARPGPlayerRegisterInfo> PlayerRegisterInfos;
};
