#include "MMOARPGDbClientObject.h"
#include "MMOARPGGateClientObject.h"

#include "Log/MMOARPGLoginServerLog.h"
#include "ServerList.h"

#include "Protocol/LoginProtocol.h" // Plugin: MMOARPGComm
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
	case SP_LoginResponses:
	{
		// Get Response Msg
		FSimpleAddrInfo AddrInfo;
		ELoginType ResponseType = ELoginType::DB_ERROR;
		//FString UserDataBack;
		//SIMPLE_PROTOCOLS_RECEIVE(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);
		SIMPLE_PROTOCOLS_RECEIVE(SP_LoginResponses, AddrInfo, ResponseType);

		UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[LoginResponse] Login Server Recived: type=%i, back_userdata=%s"),
			(uint32)ResponseType, *UserDataBack);

		// TODO: Take Best Gate Server depends on local Gate Client Objects
		FMMOARPGGateStatus GateStatus;
		if (UMMOARPGGateClientObject* GateClient = Cast<UMMOARPGGateClientObject>(GateClientA->GetController()))
		{
			int32 GateConnectionNum = GateClient->GetGateStatus().GateConnectionNum;
			if (GateConnectionNum <= 2000) // TODO: define MAX CONNECT
			{
				GateStatus = GateClient->GetGateStatus();
			}
		}

		// Forward NetFlow to User Client by Login Server
		//SIMPLE_SERVER_SEND(LoginServer, SP_LoginResponses, AddrInfo, ResponseType, UserDataBack); // add best gate status to UserDataBack
		SIMPLE_SERVER_SEND(LoginServer, SP_LoginResponses, AddrInfo, ResponseType, GateStatus); // add gate status

		break;
	}
	}
}
