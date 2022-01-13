// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGLoginServer.h"

#include "RequiredProgramMainCPPInclude.h"

#include "Global/SimpleNetGlobalInfo.h"
#include "SimpleNetManage.h"

#include "LoginServer/MMOARPGLoginServerObject.h"
#include "LoginServer/MMOARPGDbClientObject.h"
#include "LoginServer/MMOARPGGateClientObject.h"
#include "Log/MMOARPGLoginServerLog.h"
#include "ServerList.h"

IMPLEMENT_APPLICATION(MMOARPGLoginServer, "MMOARPGLoginServer");


INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("Welcome to MMO Login Server~"));

	// Init Net Channel
	FSimpleNetGlobalInfo::Get()->Init();

	// Create Gate Server Instance
	LoginServer = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_LISTEN, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	DbClient    = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	GateClientA = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);

	// Global Register Reflect Class
	LoginServer->NetworkObjectClass = UMMOARPGLoginServerObject::StaticClass();
	DbClient->NetworkObjectClass    = UMMOARPGDbClientObject::StaticClass();
	GateClientA->NetworkObjectClass = UMMOARPGGateClientObject::StaticClass();

	// Init Servers
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("MMO Login Server Initing..."));
	if (!LoginServer->Init())
	{
		delete LoginServer;
		UE_LOG(LogMMOARPGLoginServer, Error, TEXT("MMO Login Server Init failed!"));
		return INDEX_NONE;
	}
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("MMO DB Client Initing..."));
	if (!DbClient->Init(11221))
	{
		delete DbClient;
		UE_LOG(LogMMOARPGLoginServer, Error, TEXT("MMO DB Client Init failed!"));
		return INDEX_NONE;
	}
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("MMO Gate Client Initing..."));
	if (!GateClientA->Init(11222))
	{
		delete GateClientA;
		UE_LOG(LogMMOARPGLoginServer, Error, TEXT("MMO Gate Client(A) Init failed!"));
		return INDEX_NONE;
	}

	// Tick Loop per frame
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("MMO Login Server Loop Start..."));
	double LastTime = FPlatformTime::Seconds();
	while (!IsEngineExitRequested())
	{
		//FPlatformProcess::Sleep(0.033f); // TODO: remove

		double CurrentTime = FPlatformTime::Seconds();
		float DeltaS = CurrentTime - LastTime;

		LoginServer->Tick(DeltaS);
		DbClient->Tick(DeltaS);
		GateClientA->Tick(DeltaS);

		LastTime = CurrentTime;
	}

	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("MMO Login Server Destroying..."));
	FSimpleNetManage::Destroy(LoginServer);
	FSimpleNetManage::Destroy(DbClient);
	FSimpleNetManage::Destroy(GateClientA);

	FEngineLoop::AppExit();
	return 0;
}
