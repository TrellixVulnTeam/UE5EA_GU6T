// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGCenterServer.h"

#include "RequiredProgramMainCPPInclude.h"

// Plugins
#include "Global/SimpleNetGlobalInfo.h"
#include "SimpleNetManage.h"

#include "CenterServer/MMOARPGCenterServerObject.h"
#include "CenterServer/MMOARPGDbClientObject.h"
#include "Log/MMOARPGCenterServerLog.h"
#include "ServerList.h"

IMPLEMENT_APPLICATION(MMOARPGCenterServer, "MMOARPGCenterServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGCenterServer, Display, TEXT("Welcome to MMO Center Server~"));

	// Init Net Channel
	FSimpleNetGlobalInfo::Get()->Init();

	// Create Gate Server Instance
	CenterServer = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_LISTEN, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	DbClient = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);

	// Global Register Reflect Class
	CenterServer->NetworkObjectClass = UMMOARPGCenterServerObject::StaticClass();
	DbClient->NetworkObjectClass = UMMOARPGDbClientObject::StaticClass();

	// Init Servers
	UE_LOG(LogMMOARPGCenterServer, Display, TEXT("MMO Center Server Initing..."));
	if (!CenterServer->Init())
	{
		delete CenterServer;
		UE_LOG(LogMMOARPGCenterServer, Error, TEXT("MMO Center Server Init failed!"));
		return INDEX_NONE;
	}
	UE_LOG(LogMMOARPGCenterServer, Display, TEXT("MMO DB Client Initing..."));
	if (!DbClient->Init(11221))
	{
		delete DbClient;
		UE_LOG(LogMMOARPGCenterServer, Error, TEXT("MMO Db Client Init failed!"));
		return INDEX_NONE;
	}

	// Tick Loop per frame
	UE_LOG(LogMMOARPGCenterServer, Display, TEXT("MMO Center Server Loop Start..."));
	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		FPlatformProcess::Sleep(0.033f); // TODO: remove

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaS = CurrentTime - LastTime;

		CenterServer->Tick(DeltaS);
		DbClient->Tick(DeltaS);

		LastTime = CurrentTime;
	}

	UE_LOG(LogMMOARPGCenterServer, Display, TEXT("MMO Center Server Destroying..."));
	FSimpleNetManage::Destroy(CenterServer);
	FSimpleNetManage::Destroy(DbClient);

	FEngineLoop::AppExit();
	return 0;
}
