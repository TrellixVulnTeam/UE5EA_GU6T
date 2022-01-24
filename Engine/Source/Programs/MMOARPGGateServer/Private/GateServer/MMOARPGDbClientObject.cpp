#include "MMOARPGDbClientObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "SimpleProtocolsDefinition.h" // Plugin: SimpleNetChannel

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
	case SP_CharacterAppearancesResponses:
	{
		// Get Response Msg
		FSimpleAddrInfo AddrInfo;
		FString CharacterAppearancesJson;
		SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearancesResponses, AddrInfo, CharacterAppearancesJson);

		UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CharacterAppearancesResponses] DB Client Recived: character appearances=%s"),
			*CharacterAppearancesJson);

		// Forward NetFlow to User Client by Gate Server
		SIMPLE_SERVER_SEND(GateServer, SP_CharacterAppearancesResponses, AddrInfo, CharacterAppearancesJson);

		break;
	}
	default:
		break;
	}
}