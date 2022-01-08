#pragma once

#include "CoreMinimal.h"
#include "Core/SimpleMysqlLinkType.h"

struct FMysqlConfig
{
	FMysqlConfig()
		: User("root"), Host("127.0.0.1"), Passwd("root"), DB("hello"), Port(3306)
	{
		// 支持多条语句
		ClientFlags.Add(ESimpleClientFlags::Client_Multi_Statements);
		// 支持多条返回结果
		ClientFlags.Add(ESimpleClientFlags::Client_Multi_Results);
		
	}

	FString User;
	FString Host;
	FString Passwd;
	FString DB;
	int32 Port;
	TArray<ESimpleClientFlags> ClientFlags;
};