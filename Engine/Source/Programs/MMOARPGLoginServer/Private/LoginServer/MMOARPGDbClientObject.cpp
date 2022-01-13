#include "MMOARPGDbClientObject.h"

#include "Log/MMOARPGLoginServerLog.h"
#include "ServerList.h"

#include "Protocol/LoginProtocol.h" // Plugin: MMOARPGComm

void UMMOARPGDbClientObject::Init()
{
	Super::Init();
}

void UMMOARPGDbClientObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGDbClientObject::Close()
{
	Super::Close();
}

void UMMOARPGDbClientObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
	case SP_LoginResponses:
	{
		// TODO
		break;
	}
	case SP_LoginRequests:
	{
		UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[LoginRequest] DB Client Recived"));
	}
	}
}