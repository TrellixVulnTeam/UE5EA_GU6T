// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGDbServer.h"

#include "RequiredProgramMainCPPInclude.h"

#include "Global/SimpleNetGlobalInfo.h"
#include "SimpleNetManage.h"

#include "DbServer/MySQLConfig.h"
#include "DbServer/MMOARPGDbServerObject.h"
#include "Log/MMOARPGDbServerLog.h"

IMPLEMENT_APPLICATION(MMOARPGDbServer, "MMOARPGDbServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGDbServer, Display, TEXT("Welcome to MMO DB Server~"));

	// Init MySQL Config
	FSimpleMysqlConfig::Get()->Init();
	// Init Net Channel
	FSimpleNetGlobalInfo::Get()->Init();

	// Create DB Server Instance
	FSimpleNetManage* DbServer = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_LISTEN, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);

	// Global Register Reflect Class
	FSimpleChannel::SimpleControllerDelegate.BindLambda(
		[]()->UClass*
		{
			return UMMOARPGDbServerObject::StaticClass();
		}
	);

	// Init Server
	UE_LOG(LogMMOARPGDbServer, Display, TEXT("MMO DB Server Initing..."));
	if (!DbServer->Init())
	{
		delete DbServer;
		UE_LOG(LogMMOARPGDbServer, Error, TEXT("MMO DB Server Init failed!"));
		return INDEX_NONE;
	}

	// Tick Loop per frame
	UE_LOG(LogMMOARPGDbServer, Display, TEXT("MMO DB Server Loop Start..."));
	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		//FPlatformProcess::Sleep(0.033f); // TODO: remove

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaS = CurrentTime - LastTime;

		DbServer->Tick(DeltaS);

		LastTime = CurrentTime;
	}

	UE_LOG(LogMMOARPGDbServer, Display, TEXT("MMO DB Server Destroying..."));
	FSimpleNetManage::Destroy(DbServer);

	FEngineLoop::AppExit();
	return 0;
}
