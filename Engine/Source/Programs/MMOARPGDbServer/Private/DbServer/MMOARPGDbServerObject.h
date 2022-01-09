#pragma once

#include "CoreMinimal.h"
#include "UObject/SimpleController.h"
#include "Core/SimpleMysqlLinkType.h"

#include "MMOARPGDbServerObject.generated.h"

class USimpleMysqlObject;

UCLASS()
class UMMOARPGDbServerObject : public USimpleController
{
	GENERATED_BODY()

public:
	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();
	virtual void RecvProtocol(uint32 InProtocol);

public:
	bool Post(const FString& InSQL);
	bool Get(const FString& InSQL, TArray<FSimpleMysqlResult>& Results);
protected:
	USimpleMysqlObject* MysqlObjectRead;
	USimpleMysqlObject* MysqlObjectWrite;
};
