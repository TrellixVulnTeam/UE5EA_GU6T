#include "MMOARPGGateServerObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

UMMOARPGGateServerObject::UMMOARPGGateServerObject()
	: m_UserID(INDEX_NONE)
{
}

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

	if (m_UserID != INDEX_NONE)
	{
		UE_LOG(LogMMOARPGGateServer, Display, TEXT("[INFO][PlayerQuit] Player connection is off. Send PlayerQuit Requests. User ID = %i"), m_UserID);
		SIMPLE_CLIENT_SEND(CenterClient, SP_PlayerQuitRequests, m_UserID);
	}
	else
	{
		UE_LOG(LogMMOARPGGateServer, Display, TEXT("[ERROR][PlayerQuit] Player connection is off, but find User ID is NONE."));
	}
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

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[GateStatusResponses] Gate Server sended."));
			break;
		}
		case SP_CharacterAppearancesRequests:
		{
			// Get User ID
			int32 UserID = INDEX_NONE;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearancesRequests, UserID);
			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[CharacterAppearancesRequests] Gate Server Recived: user id=%i"),
				UserID);

			// Register User ID
			m_UserID = UserID;

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

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[CheckCharacterNameRequests] Gate Server Recived: user id=%i, name=%s"),
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

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[CreateCharacterRequests] Gate Server Recived: user id=%i, ca_json=%s"),
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
			SIMPLE_CLIENT_SEND(CenterClient, SP_LoginToDSServerRequests, UserID, SlotPos, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[LoginToDSServerRequests] Gate Server Recived: user id=%i, slot pos=%i"),
				UserID, SlotPos);

			break;
		}
	}
}