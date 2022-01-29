#include "MMOARPGDbClientObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

// Plugins
#include "Protocol/RoleHallProtocol.h"
#include "MMOARPGCommType.h"
#include "SimpleProtocolsDefinition.h"

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
		case SP_CheckCharacterNameResponses:
		{
			// Get Response Msg
			FSimpleAddrInfo AddrInfo;
			ECheckNameType ResponseType = ECheckNameType::UNKNOW_ERROR;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CheckCharacterNameResponses, ResponseType, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CheckCharacterNameResponses] DB Client Recived."));

			// Forward NetFlow to User Client by Gate Server
			SIMPLE_SERVER_SEND(GateServer, SP_CheckCharacterNameResponses, AddrInfo, ResponseType);

			break;
		}
		case SP_CreateCharacterResponses:
		{
			// Get Response Msg
			ECheckNameType CheckNameType = ECheckNameType::UNKNOW_ERROR;
			bool bCreateCharacter = false;
			FString CharacterAppearancesJson;
			FSimpleAddrInfo AddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CreateCharacterResponses, CheckNameType, bCreateCharacter, CharacterAppearancesJson, AddrInfo);

			UE_LOG(LogMMOARPGGateServer, Display, TEXT("[SP_CreateCharacterResponses] DB Client Recived."));

			// Forward NetFlow to User Client by Gate Server
			SIMPLE_SERVER_SEND(GateServer, SP_CreateCharacterResponses, AddrInfo, CheckNameType, bCreateCharacter, CharacterAppearancesJson);

			break;
		}
		default:
			break;
	}
}