#include "MMOARPGGateClientObject.h"

#include "Log/MMOARPGLoginServerLog.h"

#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm

UMMOARPGGateClientObject::UMMOARPGGateClientObject()
	:Time(0.f)
{
}

void UMMOARPGGateClientObject::Init()
{
	Super::Init();
}

void UMMOARPGGateClientObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Time += DeltaTime;
	if (Time >= 1.f)
	{
		Time = 0.f;
		// Sync Gate Servers Status
		SIMPLE_PROTOCOLS_SEND(SP_GateStatusRequests);
	}
}

void UMMOARPGGateClientObject::Close()
{
	Super::Close();
}

void UMMOARPGGateClientObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
	// Recive Gate Servers Status
	case SP_GateStatusResponses:
	{
		// Get Response Msg and Update local Gate Server Status
		SIMPLE_PROTOCOLS_RECEIVE(SP_GateStatusResponses, GateStatus);

		UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[SP_GateStatusResponses] DB Client Recived."));
		break;
	}
	default:
		break;
	}
}