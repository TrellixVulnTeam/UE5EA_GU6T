#include "MMOARPGCommType.h"

#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"

namespace NetDataParser
{

	void UserDataToJson(const FMMOARPGUserData& InUserData, FString& OutJson)
	{
		OutJson.Empty();
		// Use Json Writer to create json string
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJson);

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue(TEXT("ID"), *FString::FromInt(InUserData.ID));
		JsonWriter->WriteValue(TEXT("Account"), InUserData.Account);
		JsonWriter->WriteValue(TEXT("Email"), InUserData.Email);
		JsonWriter->WriteValue(TEXT("NickName"), InUserData.NickName);
		JsonWriter->WriteValue(TEXT("AvatarURL"), InUserData.AvatarURL);

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	void JsonToUserdata(const FString& InJson, FMMOARPGUserData& OutUserData)
	{
		// Read and Deserialize Json
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJson);
		TSharedPtr<FJsonObject> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			OutUserData.ID        = ReadRoot->GetIntegerField(TEXT("ID"));
			OutUserData.Account   = ReadRoot->GetStringField(TEXT("Account"));
			OutUserData.Email     = ReadRoot->GetStringField(TEXT("Email"));
			OutUserData.NickName  = ReadRoot->GetStringField(TEXT("NickName"));
			OutUserData.AvatarURL = ReadRoot->GetStringField(TEXT("AvatarURL"));
		}
	}

	MMOARPGCOMM_API void CharacterAppearancesToJson(const FMMOARPGCharacterAppearances& InCAs, FString& OutJson)
	{
		OutJson.Empty();
		// Use Json Writer to create json string
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJson);

		JsonWriter->WriteArrayStart();

		for (auto &CA : InCAs)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue(TEXT("Name"), CA.Name);
			JsonWriter->WriteValue(TEXT("CreationDate"), CA.CreationDate);
			JsonWriter->WriteValue(TEXT("Lv"), CA.Lv);
			JsonWriter->WriteValue(TEXT("SlotPos"), CA.SlotPos);
			JsonWriter->WriteValue(TEXT("LegSize"), CA.LegSize);
			JsonWriter->WriteValue(TEXT("WaistSize"), CA.WaistSize);
			JsonWriter->WriteValue(TEXT("ArmSize"), CA.ArmSize);
			//...

			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();
		JsonWriter->Close();
	}

	MMOARPGCOMM_API void JsonToCharacterAppearances(const FString& InJson, FMMOARPGCharacterAppearances& OutCAs)
	{
		// Read and Deserialize Json
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJson);
		TArray<TSharedPtr<FJsonValue>> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			for (auto &CAJsonValue : ReadRoot)
			{
				OutCAs.Add(FMMOARPGCharacterAppearance());
				FMMOARPGCharacterAppearance& CA = OutCAs.Last();

				if (TSharedPtr<FJsonObject> CAJsonObject = CAJsonValue->AsObject())
				{
					CA.Name         = CAJsonObject->GetStringField(TEXT("Name"));
					CA.CreationDate = CAJsonObject->GetStringField(TEXT("CreationDate"));
					CA.Lv           = CAJsonObject->GetIntegerField(TEXT("Lv"));
					CA.SlotPos      = CAJsonObject->GetIntegerField(TEXT("SlotPos"));
					CA.LegSize      = CAJsonObject->GetNumberField(TEXT("LegSize"));
					CA.WaistSize    = CAJsonObject->GetNumberField(TEXT("WaistSize"));
					CA.ArmSize      = CAJsonObject->GetNumberField(TEXT("ArmSize"));
					//...
				}
			}
		}
	}

	MMOARPGCOMM_API void CharacterAppearanceToJson(const FMMOARPGCharacterAppearance& InCA, FString& OutJson)
	{
		OutJson.Empty();
		// Use Json Writer to create json string
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJson);

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue(TEXT("Name"), InCA.Name);
		JsonWriter->WriteValue(TEXT("CreationDate"), InCA.CreationDate);
		JsonWriter->WriteValue(TEXT("Lv"), InCA.Lv);
		JsonWriter->WriteValue(TEXT("SlotPos"), InCA.SlotPos);
		JsonWriter->WriteValue(TEXT("LegSize"), InCA.LegSize);
		JsonWriter->WriteValue(TEXT("WaistSize"), InCA.WaistSize);
		JsonWriter->WriteValue(TEXT("ArmSize"), InCA.ArmSize);
		//...

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	MMOARPGCOMM_API void JsonToCharacterAppearance(const FString& InJson, FMMOARPGCharacterAppearance& OutCA)
	{
		// Read and Deserialize Json
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJson);
		TSharedPtr<FJsonObject> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			OutCA.Name         = ReadRoot->GetStringField(TEXT("Name"));
			OutCA.CreationDate = ReadRoot->GetStringField(TEXT("CreationDate"));
			OutCA.Lv           = ReadRoot->GetIntegerField(TEXT("Lv"));
			OutCA.SlotPos      = ReadRoot->GetIntegerField(TEXT("SlotPos"));
			OutCA.LegSize      = ReadRoot->GetNumberField(TEXT("LegSize"));
			OutCA.WaistSize    = ReadRoot->GetNumberField(TEXT("WaistSize"));
			OutCA.ArmSize      = ReadRoot->GetNumberField(TEXT("ArmSize"));
			//...
		}
	}

}

void FMMOARPGCharacterAppearance::Reset()
{
	Lv = INDEX_NONE;
	SlotPos = INDEX_NONE;
	LegSize = 0.f;
	WaistSize = 0.f;
	ArmSize = 0.f;
	Name.Empty();
	CreationDate.Empty();
}

void FMMOARPGUserData::Reset()
{
	ID = INDEX_NONE;

	Account.Empty();
	Email.Empty();
	NickName.Empty();
	AvatarURL.Empty();
}

void FMMOARPGPlayerRegisterInfo::Reset()
{
	UserData.Reset();
	CA.Reset();
}

bool FMMOARPGPlayerRegisterInfo::IsValid() const
{
	return UserData.ID != INDEX_NONE;
}
