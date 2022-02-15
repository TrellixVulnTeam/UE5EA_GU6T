#include "MMOARPGDbClientObject.h"

#include "Log/MMOARPGCenterServerLog.h"
#include "ServerList.h"
#include "MMOARPGCenterServerObject.h"

// Plugins
#include "SimpleProtocolsDefinition.h"
#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

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
		case SP_PlayerRegisterInfoResponses:
		{
			FString UserDataJson;
			FString CAJson;
			FSimpleAddrInfo GateAddrInfo;
			FSimpleAddrInfo CenterAddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_PlayerRegisterInfoResponses, UserDataJson, CAJson, GateAddrInfo, CenterAddrInfo);

			UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[PlayerRegisterInfoResponses] DB Client Reviced: user_data=%s, ca=%s"),
				*UserDataJson, *CAJson);

			// Register player's Character Info into Online List
			if (UserDataJson != TEXT("{}") && CAJson != TEXT("{}"))
			{
				if (UMMOARPGCenterServerObject* CenterServerObj = Cast<UMMOARPGCenterServerObject>(FSimpleNetManage::GetNetManageNetworkObject(CenterServer, CenterAddrInfo)))
				{
					FMMOARPGPlayerRegisterInfo PlayerRegisterInfo;
					NetDataParser::JsonToCharacterAppearance(CAJson, PlayerRegisterInfo.CA);
					NetDataParser::JsonToUserdata(UserDataJson, PlayerRegisterInfo.UserData);

					CenterServerObj->AddRegisterInfo(PlayerRegisterInfo);
					UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[INFO][PlayerRegister] register user successfully. User ID = %i"),
						PlayerRegisterInfo.UserData.ID);
				}
				else
				{
					UE_LOG(LogMMOARPGCenterServer, Display, TEXT("[ERROR][PlayerRegister] register user failed."));
				}
			}
		
			// TODO: Get DS Server info where Character is exist
			FSimpleAddr DSServerAddrInfo = FSimpleNetManage::GetSimpleAddr(TEXT("127.0.0.1"), 7777); // TODO

			// Send Response by Center Server
			SIMPLE_SERVER_SEND(CenterServer, SP_LoginToDSServerResponses, CenterAddrInfo, GateAddrInfo, DSServerAddrInfo);

			break;
		}
	}
}