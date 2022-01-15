#include "MMOARPGDbClientObject.h"

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
		FString UserDataBack;
		SIMPLE_PROTOCOLS_RECEIVE(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);

		UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[LoginResponse] Login Server Recived: type=%i, back_userdata=%s"),
			(uint32)ResponseType, *UserDataBack);

		// TODO: Get Gate Servers Info

		// Forward NetFlow to User Client by Login Server
		SIMPLE_SERVER_SEND(LoginServer, SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);

		break;
	}
	//case SP_LoginRequests:
	//{
	//	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[LoginRequest] DB Client Recived"));
	//	break;
	//}
	}
}