#pragma once
#include "MMOARPGDbServerType.h"

enum EPasswordVerification
{
	VERIFICATION_SUCCESS = 0,
	VERIFICATION_FAIL,
};

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