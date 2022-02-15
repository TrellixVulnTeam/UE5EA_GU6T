#include "MMOARPGCenterServerObject.h"

#include "Log/MMOARPGCenterServerLog.h"
#include "ServerList.h"

// Plugins
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/GameProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm


void UMMOARPGCenterServerObject::Init()
{
	Super::Init();

	// pre-allocation
	for (int32 i = 0; i < 2000; ++i)
		PlayerRegisterInfos.Add(i, FMMOARPGPlayerRegisterInfo());
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
