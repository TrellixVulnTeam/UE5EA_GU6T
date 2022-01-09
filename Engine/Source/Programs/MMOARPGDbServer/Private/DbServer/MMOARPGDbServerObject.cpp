#include "MMOARPGDbServerObject.h"

#include "Blueprint/SimpleMysqlObject.h"
#include "SimpleMySQLibrary.h"

#include "MySQLConfig.h"
#include "Log/MMOARPGDbServerLog.h"

void UMMOARPGDbServerObject::Init()
{
	Super::Init();

	MysqlObjectRead = USimpleMySQLLibrary::CreateMysqlObject(
		NULL,
		FSimpleMysqlConfig::Get()->GetInfo().User,
		FSimpleMysqlConfig::Get()->GetInfo().Host,
		FSimpleMysqlConfig::Get()->GetInfo().Passwd,
		FSimpleMysqlConfig::Get()->GetInfo().DB,
		FSimpleMysqlConfig::Get()->GetInfo().Port,
		FSimpleMysqlConfig::Get()->GetInfo().ClientFlags
	);

	MysqlObjectWrite = USimpleMySQLLibrary::CreateMysqlObject(
		NULL,
		FSimpleMysqlConfig::Get()->GetInfo().User,
		FSimpleMysqlConfig::Get()->GetInfo().Host,
		FSimpleMysqlConfig::Get()->GetInfo().Passwd,
		FSimpleMysqlConfig::Get()->GetInfo().DB,
		FSimpleMysqlConfig::Get()->GetInfo().Port,
		FSimpleMysqlConfig::Get()->GetInfo().ClientFlags
	);
}

void UMMOARPGDbServerObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGDbServerObject::Close()
{
	Super::Close();

	if (MysqlObjectRead)
	{
		MysqlObjectRead->ConditionalBeginDestroy();
		MysqlObjectRead = nullptr;
	}

	if (MysqlObjectWrite)
	{
		MysqlObjectWrite->ConditionalBeginDestroy();
		MysqlObjectWrite = nullptr;
	}
}

void UMMOARPGDbServerObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);
}

bool UMMOARPGDbServerObject::Post(const FString& InSQL)
{
	if (!InSQL.IsEmpty())
	{
		if (MysqlObjectWrite)
		{
			FString ErrMsg;
			USimpleMySQLLibrary::QueryLink(MysqlObjectWrite, InSQL, ErrMsg);

			if (ErrMsg.IsEmpty())
			{
				return true;
			}
			else
			{
				UE_LOG(LogMMOARPGDbServer, Error, TEXT("MMO DB Server Error: Post msg [%s]"), *ErrMsg);
			}
		}
	}

	return false;
}

bool UMMOARPGDbServerObject::Get(const FString& InSQL, TArray<FSimpleMysqlResult>& Results)
{
	if (!InSQL.IsEmpty())
	{
		if (MysqlObjectWrite)
		{
			FSimpleMysqlDebugResult Debug;
			Debug.bPrintToLog = true;

			FString ErrMsg;
			USimpleMySQLLibrary::QueryLinkResult(
				MysqlObjectWrite, 
				InSQL, 
				Results, 
				ErrMsg, 
				EMysqlQuerySaveType::STORE_RESULT, 
				Debug
			);

			if (ErrMsg.IsEmpty())
			{
				return true;
			}
			else
			{
				UE_LOG(LogMMOARPGDbServer, Error, TEXT("MMO DB Server Error: Get msg [%s]"), *ErrMsg);
			}
		}
	}

	return false;
}
