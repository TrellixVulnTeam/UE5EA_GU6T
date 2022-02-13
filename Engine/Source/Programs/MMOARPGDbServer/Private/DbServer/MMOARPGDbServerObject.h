#pragma once

#include "CoreMinimal.h"

#include "UObject/SimpleController.h"
#include "Core/SimpleMysqlLinkType.h"

#include "MMOARPGCommType.h"
#include "SimpleHTTPManage.h" // Plugin: SimpleHTTP

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

protected:
	UFUNCTION()
	void CheckPasswordVerifyResult(const FSimpleHttpRequest& InRequest, const FSimpleHttpResponse& InResponse, bool bLinkSuccessfull);

private:
	bool GetUserData(int32 InUserID, FString& OutJson);
	bool GetCAwithSlot(int32 InUserID, int32 InSlotPos, FString& OutJson);
	bool GetCAIDs(int32 InUserID, TArray<FString>& OutCAIDs);

	ECheckNameType CheckName(FString& InCharacterName);

	void GetSerialTArray(TCHAR* InPrefix, const TArray<FString>& InTArray, FString& OutString);

public:
	bool Post(const FString& InSQL);
	bool Get(const FString& InSQL, TArray<FSimpleMysqlResult>& Results);

protected:
	USimpleMysqlObject* MysqlObjectRead;
	USimpleMysqlObject* MysqlObjectWrite;
};
