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

}
