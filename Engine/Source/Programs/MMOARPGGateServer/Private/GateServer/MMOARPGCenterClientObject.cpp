#include "MMOARPGCenterClientObject.h"

#include "Log/MMOARPGGateServerLog.h"
#include "ServerList.h"

void UMMOARPGCenterClientObject::Init()
{
	Super::Init();
}

void UMMOARPGCenterClientObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGCenterClientObject::Close()
{
	Super::Close();
}

void UMMOARPGCenterClientObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	/*switch (InProtocol)
	{

	}*/
}