#include "MMOARPGDbClientObject.h"

#include "Log/MMOARPGCenterServerLog.h"
#include "ServerList.h"

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

	//switch (InProtocol)
	//{
	//	
	//}
}