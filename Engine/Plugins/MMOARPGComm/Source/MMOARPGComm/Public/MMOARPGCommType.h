#pragma once

#include "CoreMinimal.h"

#include "SimpleNetChannelType.h" // plugin:  SimpleNetChannel

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

struct MMOARPGCOMM_API FMMOARPGCharacterAppearance
{
	FMMOARPGCharacterAppearance()
		: Lv(INDEX_NONE), SlotPos(INDEX_NONE)
	{}

	FString Name;
	FString CreationDate;
	int32 Lv;
	int32 SlotPos;
};

typedef TArray<FMMOARPGCharacterAppearance> FMMOARPGCharacterAppearances;

// String(Json) <-> FMMOARPGUserData
namespace NetDataParser
{
	MMOARPGCOMM_API void UserDataToJson(const FMMOARPGUserData& InUserData, FString& OutJson);
	MMOARPGCOMM_API void JsonToUserdata(const FString& InJson, FMMOARPGUserData& OutUserData);

	MMOARPGCOMM_API void CharacterAppearancesToJson(const FMMOARPGCharacterAppearances& InCAs, FString& OutJson);
	MMOARPGCOMM_API void JsonToCharacterAppearances(const FString& InJson, FMMOARPGCharacterAppearances& OutCAs);

	MMOARPGCOMM_API void CharacterAppearanceToJson(const FMMOARPGCharacterAppearance& InCA, FString& OutJson);
	MMOARPGCOMM_API void JsonToCharacterAppearance(const FString& InJson, FMMOARPGCharacterAppearance& OutCA);
}