#include "MysqlConfig.h"

#include "Misc/FileHelper.h"

FSimpleMysqlConfig* FSimpleMysqlConfig::s_Instance = nullptr;

FSimpleMysqlConfig* FSimpleMysqlConfig::Get()
{
	if (s_Instance == nullptr)
	{
		s_Instance = new FSimpleMysqlConfig();
	}

	return s_Instance;
}

void FSimpleMysqlConfig::Destory()
{
	if (s_Instance != nullptr)
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

void FSimpleMysqlConfig::Init(const FString& InPath /*= FPaths::ProjectDir() / TEXT("MysqlConfig.ini")*/)
{
	TArray<FString> Content;

	// Load Config File to `Content`
	FFileHelper::LoadFileToStringArray(Content, *InPath);
	
	if (Content.Num())
	{
		// Parse & Set MySQL Config
		auto Lamabda = [&](TMap<FString,FString> &OutContent)
		{
			for (auto& Tmp : Content)
			{
				if (Tmp.Contains("[") && Tmp.Contains("]"))
				{
					Tmp.RemoveFromEnd("[");
					Tmp.RemoveFromEnd("]");

					OutContent.Add("ConfigHead", Tmp);
				}
				else
				{
					FString Key, Value;
					Tmp.Split(TEXT("="), &Key, &Value);

					OutContent.Add(Key, Value);
				}
			}
		};

		TMap<FString, FString> InConfigInfo;
		Lamabda(InConfigInfo);

		m_Config.User   = InConfigInfo["User"];
		m_Config.Host   = InConfigInfo["Host"];
		m_Config.Passwd = InConfigInfo["Passwd"];
		m_Config.DB     = InConfigInfo["DB"];
		m_Config.Port   = FCString::Atoi(*(InConfigInfo["Port"]));
	}
	else // if file is empty, generate default values
	{
		Content.Add(TEXT("[SimpleMysqlConfig]"));
		Content.Add(FString::Printf(TEXT("User=%s"),   *m_Config.User));
		Content.Add(FString::Printf(TEXT("Host=%s"),   *m_Config.Host));
		Content.Add(FString::Printf(TEXT("Passwd=%s"), *m_Config.Passwd));
		Content.Add(FString::Printf(TEXT("DB=%s"),     *m_Config.DB));
		Content.Add(FString::Printf(TEXT("Port=%d"),    m_Config.Port));

		FFileHelper::SaveStringArrayToFile(Content, *InPath);
	}
}