#pragma once

#include "CoreMinimal.h"

#include "SimpleNetChannelType.h" // plugin:  SimpleNetChannel

#include "MMOARPGCommType.generated.h"


enum class ELoginType
{
	LOGIN_SUCCESS,
	LOGIN_ACCOUNT_ERROR,
	LOGIN_PASSWORD_ERROR,
	DB_ERROR,
};

enum class ECheckNameType
{
	NAME_NOT_EXIST,
	NAME_EXIST,
	DB_ERROR,
	UNKNOW_ERROR
};

struct MMOARPGCOMM_API FMMOARPGUserData
{
	FMMOARPGUserData()
		:ID(INDEX_NONE)
	{}

	void Reset();

	int32 ID;
	FString Account;
	FString Email;
	FString NickName;
	FString AvatarURL; // optional
};

struct MMOARPGCOMM_API FMMOARPGGateStatus
{
	FMMOARPGGateStatus()
		: GateConnectionNum(INDEX_NONE)
	{}

	int32 GateConnectionNum;
	FSimpleAddrInfo GateAddrInfo;
};

USTRUCT()
struct MMOARPGCOMM_API FMMOARPGCharacterAppearance
{
	GENERATED_USTRUCT_BODY()

	FMMOARPGCharacterAppearance()
		: Lv(INDEX_NONE)
		, SlotPos(INDEX_NONE)
		, LegSize(0.f)
		, WaistSize(0.f)
		, ArmSize(0.f)
	{}

	void Reset();

	/* Basis */
	UPROPERTY()
	FString Name;
	UPROPERTY()
	FString CreationDate;
	UPROPERTY()
	int32 Lv;
	UPROPERTY()
	int32 SlotPos;

	/* KneadingFace */
	UPROPERTY()
	float LegSize;
	UPROPERTY()
	float WaistSize;
	UPROPERTY()
	float ArmSize;
};

typedef TArray<FMMOARPGCharacterAppearance> FMMOARPGCharacterAppearances;

USTRUCT()
struct FMMOARPGSimpleAttributeData
{
	GENERATED_USTRUCT_BODY()

	FMMOARPGSimpleAttributeData()
		: BaseValue(0.f), CurrentValue(0.f)
	{}

	UPROPERTY()
	float BaseValue;

	UPROPERTY()
	float CurrentValue;
};

USTRUCT()
struct MMOARPGCOMM_API FMMOARPGCharacterGameplayData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FMMOARPGSimpleAttributeData Health;

	UPROPERTY()
	FMMOARPGSimpleAttributeData MaxHealth;

	UPROPERTY()
	FMMOARPGSimpleAttributeData Mana;

	UPROPERTY()
	FMMOARPGSimpleAttributeData MaxMana;
};

struct MMOARPGCOMM_API FMMOARPGPlayerRegisterInfo
{
	void Reset();
	bool IsValid() const;

	FMMOARPGUserData UserData;
	FMMOARPGCharacterAppearance CA;
	TMap<int32, FMMOARPGCharacterGameplayData> CharacterAttributes;
};

// String(Json) <-> FMMOARPGUserData
namespace NetDataParser
{
	MMOARPGCOMM_API void UserDataToJson(const FMMOARPGUserData& InUserData, FString& OutJson);
	MMOARPGCOMM_API void JsonToUserdata(const FString& InJson, FMMOARPGUserData& OutUserData);

	MMOARPGCOMM_API void CharacterAppearancesToJson(const FMMOARPGCharacterAppearances& InCAs, FString& OutJson);
	MMOARPGCOMM_API void JsonToCharacterAppearances(const FString& InJson, FMMOARPGCharacterAppearances& OutCAs);

	MMOARPGCOMM_API void CharacterAppearanceToJson(const FMMOARPGCharacterAppearance& InCA, FString& OutJson);
	MMOARPGCOMM_API void JsonToCharacterAppearance(const FString& InJson, FMMOARPGCharacterAppearance& OutCA);

	MMOARPGCOMM_API void CharacterGameplayDataToJson(const FMMOARPGCharacterGameplayData& InCGD, FString& OutJson);
	MMOARPGCOMM_API bool JsonToCharacterGameplayData(const FString& InJson, FMMOARPGCharacterGameplayData& OutCGD);
}
