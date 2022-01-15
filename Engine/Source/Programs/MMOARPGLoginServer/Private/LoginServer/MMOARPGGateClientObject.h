#pragma once

#include "CoreMinimal.h"
#include "UObject/SimpleController.h"

#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

#include "MMOARPGGateClientObject.generated.h"

UCLASS()
class UMMOARPGGateClientObject : public USimpleController
{
	GENERATED_BODY()

public:
	UMMOARPGGateClientObject();

	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();
	virtual void RecvProtocol(uint32 InProtocol);

	inline FMMOARPGGateStatus& GetGateStatus() { return GateStatus; }

protected:
	FMMOARPGGateStatus GateStatus;
	float Time;
};
