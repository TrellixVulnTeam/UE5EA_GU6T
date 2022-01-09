// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGGateServer.h"

#include "RequiredProgramMainCPPInclude.h"

#include "Global/SimpleNetGlobalInfo.h"
#include "SimpleNetManage.h"

#include "GateServer/MMOARPGGateServerObject.h"
#include "GateServer/MMOARPGDbClientObject.h"
#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

IMPLEMENT_APPLICATION(MMOARPGGateServer, "MMOARPGGateServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGGateServer, Display, TEXT("Welcome to MMO Gate Server~"));

	// Init Net Channel
	FSimpleNetGlobalInfo::Get()->Init();

	// Create Gate Server Instance
	GateServer = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_LISTEN, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	DbClient = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	 
	// Global Register Reflect Class
	GateServer->NetworkObjectClass = UMMOARPGGateServerObject::StaticClass();
	DbClient->NetworkObjectClass   = UMMOARPGDbClientObject::StaticClass();

	// Init Servers
	UE_LOG(LogMMOARPGGateServer, Display, TEXT("MMO Gate Server Initing..."));
	if (!GateServer->Init())
	{
		delete GateServer;
		UE_LOG(LogMMOARPGGateServer, Error, TEXT("MMO Gate Server Init failed!"));
		return INDEX_NONE;
	}
	UE_LOG(LogMMOARPGGateServer, Display, TEXT("MMO Gate Server Initing..."));
	if (!DbClient->Init(11221))
	{
		delete DbClient;
		UE_LOG(LogMMOARPGGateServer, Error, TEXT("MMO Db Client Init failed!"));
		return INDEX_NONE;
	}

	// Tick Loop per frame
	UE_LOG(LogMMOARPGGateServer, Display, TEXT("MMO Gate Server Loop Start..."));
	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		//FPlatformProcess::Sleep(0.033f); // TODO: remove

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaS = CurrentTime - LastTime;

		GateServer->Tick(DeltaS);
		DbClient->Tick(DeltaS);

		LastTime = CurrentTime;
	}

	UE_LOG(LogMMOARPGGateServer, Display, TEXT("MMO Gate Server Destroying..."));
	FSimpleNetManage::Destroy(GateServer);
	FSimpleNetManage::Destroy(DbClient);

	FEngineLoop::AppExit();
	return 0;
}
