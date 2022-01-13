#include "MMOARPGLoginServerObject.h"

#include "Log/MMOARPGLoginServerLog.h"
#include "ServerList.h"

#include "Protocol/LoginProtocol.h" // Plugin: MMOARPGComm

void UMMOARPGLoginServerObject::Init()
{
	Super::Init();
}

void UMMOARPGLoginServerObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGLoginServerObject::Close()
{
	Super::Close();
}

void UMMOARPGLoginServerObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case SP_LoginRequests:
		{
			// Get Account & Password String
			FString AccountString;
			FString PasswordString;
			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginRequests, AccountString, PasswordString);
			UE_LOG(LogMMOARPGLoginServer, Display, TEXT("[LoginRequest] Login Server Recived: account=%s, passwd=%s"), 
				*AccountString, *PasswordString);

			{
				// TODO: verify
			}

			// Get current Login Server address
			FSimpleAddrInfo AddrInfo;
			GetRemoteAddrInfo(AddrInfo);

			// Forward NetFlow to DB Server by DB Client
			SIMPLE_CLIENT_SEND(DbClient, SP_LoginRequests, AccountString, PasswordString, AddrInfo);

			break;
		}
	}
}