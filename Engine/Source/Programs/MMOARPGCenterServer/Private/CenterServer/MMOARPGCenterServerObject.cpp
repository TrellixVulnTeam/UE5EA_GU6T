#include "MMOARPGCenterServerObject.h"
#include "Log/MMOARPGCenterServerLog.h"

// Plugins
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/GameProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm


void UMMOARPGCenterServerObject::Init()
{
	Super::Init();
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
				// TODO: Register player's Character into Online List

				// TODO: Get DS Server info where Character is exist

				// Send Response
				FSimpleAddr DSServerAddr;
				SIMPLE_PROTOCOLS_SEND(SP_LoginToDSServerResponses, GateAddrInfo, DSServerAddr);
			}

			break;
		}
	}
}
