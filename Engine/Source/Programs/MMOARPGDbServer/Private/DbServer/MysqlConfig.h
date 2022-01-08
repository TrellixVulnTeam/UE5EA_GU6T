#pragma once
#include "MMOARPGDbServerType.h"

// singleton
class FSimpleMysqlConfig
{
public:
	static FSimpleMysqlConfig* Get();
	static void Destory();

	void Init(const FString& InPath = FPaths::ProjectDir() / TEXT("MysqlConfig.ini"));
	inline const FMysqlConfig& GetInfo() const { return m_Config; }

private:
	static FSimpleMysqlConfig* s_Instance;
	FMysqlConfig m_Config;
};