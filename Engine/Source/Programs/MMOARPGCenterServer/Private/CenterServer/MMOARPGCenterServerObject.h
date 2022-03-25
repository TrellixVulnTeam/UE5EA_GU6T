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

public:
	static void AddRegisterInfo(const FMMOARPGPlayerRegisterInfo& InRegisterInfo);
	static bool RemoveRegisterInfo(const int32 InUserID);

	static void AddRegisterInfo_CharacterAttribute(int32 InUserID, int32 InCharacterID, const FMMOARPGCharacterGameplayData& InCGD);

private:
	static TMap<int32, FMMOARPGPlayerRegisterInfo> PlayerRegisterInfos;
};
