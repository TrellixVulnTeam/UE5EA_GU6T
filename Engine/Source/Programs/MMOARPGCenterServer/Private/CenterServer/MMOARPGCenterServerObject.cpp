#include "MMOARPGCenterServerObject.h"

#include "Log/MMOARPGCenterServerLog.h"
#include "ServerList.h"

// Plugins
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/GameProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm


TMap<int32, FMMOARPGPlayerRegisterInfo> UMMOARPGCenterServerObject::PlayerRegisterInfos;

void UMMOARPGCenterServerObject::Init()
{
	Super::Init();

	if (!PlayerRegisterInfos.Num())
	{
		// pre-allocation
		for (int32 i = 0; i < 2000; ++i)
			PlayerRegisterInfos.Add(i, FMMOARPGPlayerRegisterInfo());
	}
}

void UMMOARPGCenterServerObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGCenterServerObject::Close()
{
	Super::Close();
}

void UMMOARPGCenterServerObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case SP_LoginToDSServerRequests:
		{
			int32 UserID = INDEX_NONE;
			int32 SlotPos = INDEX_NONE;
			FSimpleAddrInfo GateAddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginToDSServerRequests, UserID, SlotPos, GateAddrInfo);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[LoginToDSServerRequests] Center Server Reviced: user ID=%i, slot pos=%i"), 
				UserID, SlotPos);
			
			if (UserID != INDEX_NONE && SlotPos != INDEX_NONE)
			{
				// Get current Center Server address
				FSimpleAddrInfo CenterAddrInfo;
				GetRemoteAddrInfo(CenterAddrInfo);

				// Request player's Character info
				SIMPLE_CLIENT_SEND(DbClient, SP_PlayerRegisterInfoRequests, UserID, SlotPos, GateAddrInfo, CenterAddrInfo);
			}

			break;
		}
		case SP_PlayerQuitRequests:
		{
			int32 UserID = INDEX_NONE;
			SIMPLE_PROTOCOLS_RECEIVE(SP_PlayerQuitRequests, UserID);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[PlayerQuitRequests] Center Server Reviced: user ID=%i"), UserID);

			if (UserID != INDEX_NONE)
			{
				if (RemoveRegisterInfo(UserID))
				{
					UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[INFO][PlayerQuit] remove user successfully. User ID = %i"), UserID);
				}
				else
				{
					UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[WARN][PlayerQuit] remove user not found. User ID = %i"), UserID);
				}
			}

			break;
		}
		case SP_GetLoggedCharacterCaRequests:
		{
			int32 UserID = INDEX_NONE;
			SIMPLE_PROTOCOLS_RECEIVE(SP_GetLoggedCharacterCaRequests, UserID);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[GetLoggedCharacterCaRequests] Center Server Reviced: user ID=%i"), UserID);

			if (UserID != INDEX_NONE)
			{
				for (auto& RegisterInfo : PlayerRegisterInfos)
				{
					if (RegisterInfo.Value.UserData.ID == UserID)
					{
						FString CAJson;
						NetDataParser::CharacterAppearanceToJson(RegisterInfo.Value.CA, CAJson);

						SIMPLE_PROTOCOLS_SEND(SP_GetLoggedCharacterCaResponses, UserID, CAJson);

						break;
					}
				}
			}

			break;
		}
		case SP_GetCharacterGameplayDataRequests:
		{
			int32 UserID = INDEX_NONE;
			int32 CharacterID = INDEX_NONE;
			SIMPLE_PROTOCOLS_RECEIVE(SP_GetCharacterGameplayDataRequests, UserID, CharacterID);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[GetCharacterGameplayDataRequests] Center Server Reviced: user ID=%i, character ID=%i"),
				UserID, CharacterID);

			if (UserID != INDEX_NONE && CharacterID != INDEX_NONE)
			{
				FString CharacterGameplayDataJson;
				// TODO
				if (FMMOARPGPlayerRegisterInfo* PlayerRegisterInfo = PlayerRegisterInfos.Find(UserID)) 
				{
					if (FMMOARPGCharacterGameplayData* CharacterGameplayData = PlayerRegisterInfo->CharacterAttributes.Find(CharacterID)) // data is in Center Server cache
					{
						NetDataParser::CharacterGameplayDataToJson(*CharacterGameplayData, CharacterGameplayDataJson);

						UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[GetCharacterGameplayDataRequests] Get User(%i) Character(%i) successfully."),
							UserID, CharacterID);

						SIMPLE_PROTOCOLS_SEND(SP_GetCharacterGameplayDataResponses, UserID, CharacterGameplayDataJson);
					}
					else // data isn't in Center Server cache
					{
						// Get current Center Server address
						FSimpleAddrInfo CenterAddrInfo;
						GetRemoteAddrInfo(CenterAddrInfo);

						UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[GetCharacterGameplayDataRequests] User(%i) Character(%i) cache isn't in server, sending request to DB."), UserID, CharacterID);

						// transfer request to DB Server
						SIMPLE_CLIENT_SEND(DbClient, SP_GetCharacterGameplayDataRequests, UserID, CharacterID, CenterAddrInfo);
					}
				}
				else // user isn't register to Center Server
				{
					UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[GetCharacterGameplayDataRequests] User(%i) isn't exist in server"),
						UserID);

					SIMPLE_PROTOCOLS_SEND(SP_GetCharacterGameplayDataResponses, UserID, CharacterGameplayDataJson); // response empty json
				}
			}

			break;
		}
	}
}

void UMMOARPGCenterServerObject::AddRegisterInfo(const FMMOARPGPlayerRegisterInfo& InRegisterInfo)
{
	if (!InRegisterInfo.IsValid())
		return;

	for (auto& RegisterInfo : PlayerRegisterInfos)
	{
		if (!RegisterInfo.Value.IsValid())
		{
			RegisterInfo.Value = InRegisterInfo;
			break;
		}
	}
}

bool UMMOARPGCenterServerObject::RemoveRegisterInfo(const int32 InUserID)
{
	if (InUserID == INDEX_NONE)
		return false;

	for (auto& RegisterInfo : PlayerRegisterInfos)
	{
		if (RegisterInfo.Value.UserData.ID == InUserID)
		{
			RegisterInfo.Value.Reset();
			return true;
		}
	}

	return false;
}

void UMMOARPGCenterServerObject::AddRegisterInfo_CharacterAttribute(int32 InUserID, int32 InCharacterID, const FMMOARPGCharacterGameplayData& InCGD)
{
	if (FMMOARPGPlayerRegisterInfo* PlayerRegisterInfo = PlayerRegisterInfos.Find(InUserID))
	{
		if (!PlayerRegisterInfo->CharacterAttributes.Contains(InCharacterID))
		{
			PlayerRegisterInfo->CharacterAttributes.Add(InCharacterID, InCGD);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[INFO] Add CharacterGameplayData succesfully. user_id=%i, character_i=%i"), InUserID, InCharacterID);
		}
		else
		{
			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[WARN] Adding already existed CharacterGameplayData! user_id=%i, character_i=%i"), InUserID, InCharacterID);
		}
	}
}
