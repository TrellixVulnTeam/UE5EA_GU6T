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
		case SP_CheckCharacterNameRequests:
		{
			// Get User ID & Character Name
			int32 UserID = INDEX_NONE;
			FString CharacterName;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CheckCharacterNameRequests, UserID, CharacterName);

			// Get current Gate Server address
			FSimpleAddrInfo AddrInfo;
			GetRemoteAddrInfo(AddrInfo);

			// Forward NetFlow to DB Server by DB Client
			SIMPLE_CLIENT_SEND(DbClient, SP_CheckCharacterNameRequests, UserID, CharacterName, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CheckCharacterNameRequests] Gate Server Recived: user id=%i, name=%s"),
				UserID, *CharacterName);

			break;
		}
		case SP_CreateCharacterRequests:
		{
			// Get User ID & CA
			int32 UserID = INDEX_NONE;
			FString CAJson;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CreateCharacterRequests, UserID, CAJson);

			// Get current Gate Server address
			FSimpleAddrInfo AddrInfo;
			GetRemoteAddrInfo(AddrInfo);

			// Forward NetFlow to DB Server by DB Client
			SIMPLE_CLIENT_SEND(DbClient, SP_CreateCharacterRequests, UserID, CAJson, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CreateCharacterRequests] Gate Server Recived: user id=%i, ca_json=%s"),
				UserID, *CAJson);

			break;
		}
		case SP_LoginToDSServerRequests:
		{
			// Get User ID & SlotPos
			int32 UserID = INDEX_NONE;
			int32 SlotPos = INDEX_NONE;
			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginToDSServerRequests, UserID, SlotPos);

			// Get current Gate Server address
			FSimpleAddrInfo AddrInfo;
			GetRemoteAddrInfo(AddrInfo);

			// Forward NetFlow to Center Server
			SIMPLE_CLIENT_SEND(CenterClient, SP_CreateCharacterRequests, UserID, SlotPos, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_LoginToDSServerRequests] Gate Server Recived: user id=%i, slot pos=%i"),
				UserID, SlotPos);

			break;
		}
	}
}