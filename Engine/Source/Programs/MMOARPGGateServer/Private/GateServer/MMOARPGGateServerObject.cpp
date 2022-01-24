#include "MMOARPGGateServerObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

void UMMOARPGGateServerObject::Init()
{
	Super::Init();
}

void UMMOARPGGateServerObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGGateServerObject::Close()
{
	Super::Close();
}

void UMMOARPGGateServerObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
	case SP_GateStatusRequests:
	{
		FMMOARPGGateStatus ServerStatus;
		// Get AddrInfo of this Server
		GetServerAddrInfo(ServerStatus.GateAddrInfo);
		// Get current Connection Num
		ServerStatus.GateConnectionNum = GetManage()->GetConnetionNum();

		// Send back Status to Login Server
		SIMPLE_PROTOCOLS_SEND(SP_GateStatusResponses, ServerStatus);

		UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_GateStatusResponses] Gate Server sended."));
		break;
	}
	case SP_CharacterAppearancesRequests:
	{
		// Get User ID
		int32 UserID = INDEX_NONE;
		SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearancesRequests, UserID);
		UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CharacterAppearancesRequests] Gate Server Recived: user id=%i"),
			UserID);

		// Get current Gate Server address
		FSimpleAddrInfo AddrInfo;
		GetRemoteAddrInfo(AddrInfo);

		// Forward NetFlow to DB Server by DB Client
		SIMPLE_CLIENT_SEND(DbClient, SP_CharacterAppearancesRequests, UserID, AddrInfo);

		break;
	}
	default:
		break;
	}
}