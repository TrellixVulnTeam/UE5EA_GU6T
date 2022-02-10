#include "MMOARPGCenterServerObject.h"
#include "Log/MMOARPGCenterServerLog.h"

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

	//switch (InProtocol)
	//{
	//}
}
