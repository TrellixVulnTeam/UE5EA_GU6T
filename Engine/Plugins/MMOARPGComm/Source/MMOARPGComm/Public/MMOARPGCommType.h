#pragma once

#include "CoreMinimal.h"

#include "SimpleNetChannelType.h" // plugin:  SimpleNetChannel

enum ELoginType
{
	LOGIN_SUCCESS,
	LOGIN_ACCOUNT_ERROR,
	LOGIN_PASSWORD_ERROR,
	DB_ERROR,
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

// String(Json) <-> FMMOARPGUserData
namespace NetDataParser
{
	MMOARPGCOMM_API void UserDataToJson(const FMMOARPGUserData& InUserData, FString& OutJson);
	MMOARPGCOMM_API void JsonToUserdata(const FString& InJson, FMMOARPGUserData& OutUserData);
}