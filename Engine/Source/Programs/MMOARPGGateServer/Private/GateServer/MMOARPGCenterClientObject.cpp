#include "MMOARPGCenterClientObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

// Plugins
#include "SimpleProtocolsDefinition.h"
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/GameProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

void UMMOARPGCenterClientObject::Init()
{
	Super::Init();
}

void UMMOARPGCenterClientObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGCenterClientObject::Close()
{
	Super::Close();
}

void UMMOARPGCenterClientObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case SP_LoginToDSServerResponses:
		{
			// Get Response Msg
			FSimpleAddrInfo GateAddrInfo;
			FSimpleAddr DSAddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginToDSServerResponses, GateAddrInfo, DSAddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[LoginToDSServerResponses] DB Client Recived: DS ip = %i, DS port = %i"),
				DSAddrInfo.IP, DSAddrInfo.Port);

			// Forward NetFlow to User Client by Gate Server
			SIMPLE_SERVER_SEND(GateServer, SP_LoginToDSServerResponses, GateAddrInfo, DSAddrInfo);

			break;
		}
	}
}