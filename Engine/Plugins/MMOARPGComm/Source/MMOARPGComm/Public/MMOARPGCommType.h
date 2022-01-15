#pragma once

#include "CoreMinimal.h"

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

// String(Json) <-> FMMOARPGUserData
namespace NetDataParser
{
	void UserDataToJson(const FMMOARPGUserData& InUserData, FString& OutJson);
	void JsonToUserdata(const FString& InJson, FMMOARPGUserData& OutUserData);
}