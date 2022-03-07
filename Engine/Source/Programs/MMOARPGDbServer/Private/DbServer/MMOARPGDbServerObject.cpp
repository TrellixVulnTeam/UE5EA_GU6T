#include "MMOARPGDbServerObject.h"

#include "Blueprint/SimpleMysqlObject.h" // Plugin: SimpleMySQL
#include "SimpleMySQLibrary.h" // Plugin: SimpleMySQL
#include "Global/SimpleNetGlobalInfo.h" // Plugin: SimpleNetChannel
#include "Protocol/LoginProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/RoleHallProtocol.h" // Plugin: MMOARPGComm
#include "Protocol/ServerProtocol.h" // Plugin: MMOARPGComm
#include "MMOARPGCommType.h" // Plugin: MMOARPGComm

#include "MySQLConfig.h"
#include "Log/MMOARPGDbServerLog.h"

void UMMOARPGDbServerObject::Init()
{
	Super::Init();

	MysqlObjectRead = USimpleMySQLLibrary::CreateMysqlObject(
		NULL,
		FSimpleMysqlConfig::Get()->GetInfo().User,
		FSimpleMysqlConfig::Get()->GetInfo().Host,
		FSimpleMysqlConfig::Get()->GetInfo().Passwd,
		FSimpleMysqlConfig::Get()->GetInfo().DB,
		FSimpleMysqlConfig::Get()->GetInfo().Port,
		FSimpleMysqlConfig::Get()->GetInfo().ClientFlags
	);

	MysqlObjectWrite = USimpleMySQLLibrary::CreateMysqlObject(
		NULL,
		FSimpleMysqlConfig::Get()->GetInfo().User,
		FSimpleMysqlConfig::Get()->GetInfo().Host,
		FSimpleMysqlConfig::Get()->GetInfo().Passwd,
		FSimpleMysqlConfig::Get()->GetInfo().DB,
		FSimpleMysqlConfig::Get()->GetInfo().Port,
		FSimpleMysqlConfig::Get()->GetInfo().ClientFlags
	);

	// Init Character Appearance Table in MySQL
	FString CreateCharacterCA_SQL = TEXT("CREATE TABLE IF NOT EXISTS `mmoarpg_characters_ca`(\
		`id` INT UNSIGNED AUTO_INCREMENT,\
		`mmoarpg_name` VARCHAR(100) NOT NULL,\
		`mmoarpg_date` VARCHAR(100) NOT NULL,\
		`mmoarpg_slot` INT NOT NULL,\
		`leg_size` double(11,4) DEFAULT '0.00',\
		`waist_size` double(11,4) DEFAULT '0.00',\
		`arm_size` double(11,4) DEFAULT '0.00',\
		PRIMARY KEY(`ID`)\
		) ENGINE = INNODB DEFAULT CHARSET = utf8mb4; ");

	if (!Post(CreateCharacterCA_SQL))
	{
		UE_LOG(LogMMOARPGDbServer, Display, TEXT("DB Table `Character_CA` Creation Failed!"));
	}
}

void UMMOARPGDbServerObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UMMOARPGDbServerObject::Close()
{
	Super::Close();

	if (MysqlObjectRead)
	{
		MysqlObjectRead->ConditionalBeginDestroy();
		MysqlObjectRead = nullptr;
	}

	if (MysqlObjectWrite)
	{
		MysqlObjectWrite->ConditionalBeginDestroy();
		MysqlObjectWrite = nullptr;
	}
}

void UMMOARPGDbServerObject::RecvProtocol(uint32 InProtocol)
{
	Super::RecvProtocol(InProtocol);

	switch (InProtocol)
	{
		case SP_LoginRequests:
		{
			// Get Account & Password & LoginServer Addr
			FString AccountString;
			FString PasswordString;
			FSimpleAddrInfo AddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_LoginRequests, AccountString, PasswordString, AddrInfo);
			UE_LOG(LogMMOARPGDbServer, Display, TEXT("[LoginRequest] Reviced: account=%s, passwd=%s"), *AccountString, *PasswordString);

			FString EmptyJson = TEXT("{}");

			// verify
			FString SQL = FString::Printf(TEXT("SELECT ID, user_pass FROM wp_users WHERE user_login = '%s' or user_email = '%s';"), 
				*AccountString, *AccountString);
			TArray<FSimpleMysqlResult> Results;
			// send account verify SQL to DB
			if (Get(SQL, Results))
			{
				// account verify successful
				if (Results.Num() > 0)
				{
					for (auto& Result : Results)
					{
						int32 UserID = 0;
						if (FString* IDString = Result.Rows.Find(TEXT("ID")))
						{
							UserID = FCString::Atoi(**IDString);
						}
						// send password verify HTTP query to Wordpress
						if (FString* UserPass = Result.Rows.Find(TEXT("user_pass")))
						{
							// http://192.168.50.149/passwdverify.php?UserID=1&EncryptedPassword=$P$Bi6eytHd7TezOoXlZC.YMIrnz7t/8K1&Password=admin&IP=192.168.0.1&Port=8080&Channel=1
							FString WordpressIp = FSimpleNetGlobalInfo::Get()->GetInfo().PublicIP;
							FString PasswdVerifyURL = FString::Printf(TEXT("http://%s/passwdverify.php"), *WordpressIp);
							FString PasswdVerifyParm = FString::Printf(TEXT("EncryptedPassword=%s&Password=%s&IP=%i&Port=%i&Channel=%s&UserID=%i"),
								**UserPass,
								*PasswordString,
								AddrInfo.Addr.IP,
								AddrInfo.Addr.Port,
								*AddrInfo.ChannelID.ToString(),
								UserID
							);

							FSimpleHTTPResponseDelegate Delegate;
							Delegate.SimpleCompleteDelegate.BindUObject(this, &UMMOARPGDbServerObject::CheckPasswordVerifyResult);

							SIMPLE_HTTP.PostRequest(*PasswdVerifyURL, *PasswdVerifyParm, Delegate);
						}
					}
				}
				else
				{
					ELoginType ResponseType = ELoginType::LOGIN_ACCOUNT_ERROR;
					SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, EmptyJson);
				}
			}
			else
			{
				ELoginType ResponseType = ELoginType::DB_ERROR;
				SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, EmptyJson);
			}		
			break;
		}
		case SP_CharacterAppearancesRequests:
		{
			// Get User ID
			int32 UserID = INDEX_NONE;
			FSimpleAddrInfo AddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CharacterAppearancesRequests, UserID, AddrInfo);
			UE_LOG(LogMMOARPGDbServer, Display, TEXT("[SP_CharacterAppearancesRequests] DB Server Recived: user id=%i"),
				UserID);

			if (UserID > 0)
			{
				/* Step1: Get Character IDs metadata */
				FString CAIDsString;

				TArray<FString> CAIDs;
				if (GetCAIDs(UserID, CAIDs))
					GetSerialTArray(TEXT(","), CAIDs, CAIDsString);

				/* Step2: Get Character Appearances by IDs */
				FMMOARPGCharacterAppearances CharacterAppearances;

				if (!CAIDsString.IsEmpty())
				{
					FString GetCASQL = FString::Printf(TEXT("SELECT * FROM mmoarpg_characters_ca WHERE id IN (%s);"), *CAIDsString);
					TArray<FSimpleMysqlResult> GetCASQLResults;
					if (Get(GetCASQL, GetCASQLResults))
					{
						// get character appearances
						if (GetCASQLResults.Num() > 0)
						{
							for (auto& GetCASQLResult : GetCASQLResults)
							{
								CharacterAppearances.Add(FMMOARPGCharacterAppearance());
								FMMOARPGCharacterAppearance& NewCA = CharacterAppearances.Last();

								if (FString* Name = GetCASQLResult.Rows.Find(TEXT("mmoarpg_name")))
								{
									NewCA.Name = *Name;
								}
								if (FString* Date = GetCASQLResult.Rows.Find(TEXT("mmoarpg_date")))
								{
									NewCA.CreationDate = *Date;
								}
								if (FString* Slot = GetCASQLResult.Rows.Find(TEXT("mmoarpg_slot")))
								{
									NewCA.SlotPos = FCString::Atoi(**Slot);
								}
								if (FString* LegSize = GetCASQLResult.Rows.Find(TEXT("leg_size")))
								{
									NewCA.LegSize = FCString::Atof(**LegSize);
								}
								if (FString* WaistSize = GetCASQLResult.Rows.Find(TEXT("waist_size")))
								{
									NewCA.WaistSize = FCString::Atof(**WaistSize);
								}
								if (FString* ArmSize = GetCASQLResult.Rows.Find(TEXT("arm_size")))
								{
									NewCA.ArmSize = FCString::Atof(**ArmSize);
								}
							}
						}
						// didn't get any character appearance
						else
						{
						}
					}
					else
					{
					}
				}

				FString CharacterAppearancesJson;
				NetDataParser::CharacterAppearancesToJson(CharacterAppearances, CharacterAppearancesJson);

				SIMPLE_PROTOCOLS_SEND(SP_CharacterAppearancesResponses, AddrInfo, CharacterAppearancesJson);
			}

			break;
		}
		case SP_CheckCharacterNameRequests:
		{
			// Get User ID & Character Name
			int32 UserID = INDEX_NONE;
			FString CharacterName;
			FSimpleAddrInfo AddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CheckCharacterNameRequests, UserID, CharacterName, AddrInfo);

			UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CheckCharacterNameRequests] DB Server Recived: user id=%i, name=%s"),
				UserID, *CharacterName);

			ECheckNameType ResponseType = ECheckNameType::UNKNOW_ERROR;
			if (UserID > 0)
			{
				ResponseType = CheckName(CharacterName);
			}

			SIMPLE_PROTOCOLS_SEND(SP_CheckCharacterNameResponses, ResponseType, AddrInfo);

			break;
		}
		case SP_CreateCharacterRequests:
		{
			// Get User ID
			int32 UserID = INDEX_NONE;
			FString CAJson;
			FSimpleAddrInfo AddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_CreateCharacterRequests, UserID, CAJson, AddrInfo);

			UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] DB Server Recived: user id=%i, ca_json=%s"),
				UserID, *CAJson);

			if (UserID != INDEX_NONE)
			{
				// Deserialize character appearance json
				FMMOARPGCharacterAppearance CA;
				NetDataParser::JsonToCharacterAppearance(CAJson, CA);

				if (CA.SlotPos >= 0 && CA.SlotPos < 3 
					&& CA.Lv == INDEX_NONE
					&& CA.LegSize <= 10.f && CA.LegSize >= 0.f
					&& CA.WaistSize <= 10.f && CA.WaistSize >= 0.f
					&& CA.ArmSize <= 10.f && CA.ArmSize >= 0.f) // verify character appearance data
				{
					bool bCreateCharacter = false;
					ECheckNameType CheckNameType = CheckName(CA.Name);
					if (CheckNameType == ECheckNameType::NAME_NOT_EXIST)
					{
						bool bIsHasCA = false;
						/* Step1: Get Character IDs metadata */
						TArray<FString> CAIDs;

						FString GetCAIDSQL = FString::Printf(TEXT("SELECT meta_value FROM wp_usermeta WHERE user_id = %i and meta_key = 'character_ca_id';"), UserID);
						UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] excute Get CA ID SQL: %s"),
							*GetCAIDSQL);

						TArray<FSimpleMysqlResult> GetCAIDResults;
						if (Get(GetCAIDSQL, GetCAIDResults))
						{
							UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] Get CA ID SQL success."));
							// get character slots
							if (GetCAIDResults.Num() > 0)
							{
								for (auto& GetCAIDResult : GetCAIDResults)
								{
									if (FString* CAIDsString = GetCAIDResult.Rows.Find(TEXT("meta_value")))
									{
										CAIDsString->ParseIntoArray(CAIDs, TEXT("|"));
									}
								}

								bIsHasCA = true;
							}
							// this user haven't create any character
							else
							{
							}

							bCreateCharacter = true;
						}
						else
						{
						}

						/* Step2: Insert Character Appearance data into DB */
						if (bCreateCharacter)
						{
							FString InsertCASQL = FString::Printf(TEXT("INSERT INTO mmoarpg_characters_ca (mmoarpg_name, mmoarpg_date, mmoarpg_slot, leg_size, waist_size, arm_size) VALUES('%s', '%s', %i, %.2lf, %.2lf, %.2lf);"),
								*CA.Name, *CA.CreationDate, CA.SlotPos, CA.LegSize, CA.WaistSize, CA.ArmSize);
							UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] excute Insert CA SQL: %s"),
								*InsertCASQL);

							if (Post(InsertCASQL))
							{
								UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] Insert CA SQL success."));
								FString GetCaIdSQL = FString::Printf(TEXT("SELECT id FROM mmoarpg_characters_ca WHERE mmoarpg_name = '%s';"), *CA.Name);

								TArray<FSimpleMysqlResult> GetCaIdResults;
								if (Get(GetCaIdSQL, GetCaIdResults))
								{
									// insert successfully
									if (GetCaIdResults.Num() > 0)
									{
										for (auto& GetCaIdResult : GetCaIdResults)
										{
											if (FString* IDString = GetCaIdResult.Rows.Find(TEXT("id")))
											{
												CAIDs.Add(*IDString);
											}
										}
									}
									// insert unknow error.
									else
									{
									}
								}
								else
								{
									bCreateCharacter = false;
								}
							}
							else
							{
								bCreateCharacter = false;
							}
						}

						/* Step3: Update Character metadata */
						if (bCreateCharacter)
						{
							FString NewCAIDsString;
							for (auto& ID : CAIDs)
							{
								NewCAIDsString += ID;
								NewCAIDsString += TEXT("|");
							}
							NewCAIDsString.RemoveFromEnd(TEXT("|"));

							FString UpdateMetaSQL;
							if (bIsHasCA)
							{
								UpdateMetaSQL = FString::Printf(TEXT("UPDATE wp_usermeta SET meta_value = '%s' WHERE meta_key = 'character_ca_id' AND user_id = %i;"),
									*NewCAIDsString, UserID);
							}
							else
							{
								UpdateMetaSQL = FString::Printf(TEXT("INSERT INTO wp_usermeta (user_id, meta_key, meta_value) VALUES(%i, 'character_ca_id', '%s');"),
									UserID, *NewCAIDsString);
							}
							UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] excute Update Metadata SQL: %s"),
								*UpdateMetaSQL);

							if (!Post(UpdateMetaSQL))
							{
								bCreateCharacter = false;
							}
							else
							{
								UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] Update Metadata SQL success."));
							}
						}
					}
					
					UE_LOG(LogMMOARPGDbServer, Display, TEXT("[CreateCharacterRequests] Sending back rst. Done."));
					SIMPLE_PROTOCOLS_SEND(SP_CreateCharacterResponses, CheckNameType, bCreateCharacter, CAJson, AddrInfo);
				}

			}

			break;
		}
		case SP_PlayerRegisterInfoRequests:
		{
			int32 UserID = INDEX_NONE;
			int32 SlotPos = INDEX_NONE;
			FSimpleAddrInfo GateAddrInfo;
			FSimpleAddrInfo CenterAddrInfo;
			SIMPLE_PROTOCOLS_RECEIVE(SP_PlayerRegisterInfoRequests, UserID, SlotPos, GateAddrInfo, CenterAddrInfo);

			UE_LOG(LogMMOARPGDbServer, Display, TEXT("[PlayerRegisterInfoRequests] DB Server Recived: user id=%i, slot pos=%i"),
				UserID, SlotPos);

			if (UserID != INDEX_NONE && SlotPos != INDEX_NONE)
			{
				// Get User Data & CA
				FString UserDataJson;
				FString CAJson;
				if (GetUserData(UserID, UserDataJson) && GetCAwithSlot(UserID, SlotPos, CAJson))
				{
					SIMPLE_PROTOCOLS_SEND(SP_PlayerRegisterInfoResponses, UserDataJson, CAJson, GateAddrInfo, CenterAddrInfo);
				}
				else
				{
					UserDataJson = TEXT("{}");
					CAJson = TEXT("{}");

					SIMPLE_PROTOCOLS_SEND(SP_PlayerRegisterInfoResponses, UserDataJson, CAJson, GateAddrInfo, CenterAddrInfo);
				}
			}
		}

	}
}

void UMMOARPGDbServerObject::CheckPasswordVerifyResult(const FSimpleHttpRequest& InRequest, const FSimpleHttpResponse& InResponse, bool bLinkSuccessfull)
{
	if (bLinkSuccessfull)
	{
		// Get Passsword Verify Message: `ID&IP&Port&Channel&0`
		TArray<FString> Values;
		InResponse.ResponseMessage.ParseIntoArray(Values, TEXT("&"));

		// Parse Passsword Verify Message
		FSimpleAddrInfo AddrInfo;
		uint32 UserID = 0;
		EPasswordVerification PV = EPasswordVerification::VERIFICATION_FAIL;
		if (Values.Num())
		{
			// get user ID
			if (Values.IsValidIndex(0))
			{
				UserID = FCString::Atoi(*Values[0]);
			}
			// get login server IP
			if (Values.IsValidIndex(1))
			{
				AddrInfo.Addr.IP = FCString::Atoi(*Values[1]);
			}
			// get login server Port
			if (Values.IsValidIndex(2))
			{
				AddrInfo.Addr.Port = FCString::Atoi(*Values[2]);
			}
			// get ChannelID (GUID)
			if (Values.IsValidIndex(3))
			{
				FGuid::ParseExact(Values[3], EGuidFormats::Digits, AddrInfo.ChannelID);
			}
			// get Password Verification Result
			if (Values.IsValidIndex(4))
			{
				PV = (EPasswordVerification)FCString::Atoi(*Values[4]);
			}

			// TODO: Send back user data
			FString UserDataJson = TEXT("{}");

			if (PV == VERIFICATION_SUCCESS)
			{
				// Password Verification is Success
				if (UserID != 0)
				{
					if (GetUserData(UserID, UserDataJson))
					{
						ELoginType ResponseType = ELoginType::LOGIN_SUCCESS;
						SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataJson);
					}
					else
					{
						ELoginType ResponseType = ELoginType::DB_ERROR;
						SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataJson);
					}
				}
			}
			else
			{
				// Password Verification is Fail
				ELoginType ResponseType = ELoginType::LOGIN_PASSWORD_ERROR;
				SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataJson);
			}
		}
	}
}

ECheckNameType UMMOARPGDbServerObject::CheckName(FString& InCharacterName)
{
	ECheckNameType ResponseType = ECheckNameType::UNKNOW_ERROR;
	
	if (!InCharacterName.IsEmpty())
	{
		// Check Character Name
		FString CheckSQL = FString::Printf(TEXT("SELECT id FROM mmoarpg_characters_ca WHERE mmoarpg_name = \"%s\";"), *InCharacterName);

		TArray<FSimpleMysqlResult> Results;
		// send character name verify SQL to DB
		if (Get(CheckSQL, Results))
		{
			// character name is existed
			if (Results.Num() > 0)
			{
				ResponseType = ECheckNameType::NAME_EXIST;
			}
			// character name isn't existed
			else
			{
				ResponseType = ECheckNameType::NAME_NOT_EXIST;
			}
		}
		else
		{
			ResponseType = ECheckNameType::DB_ERROR;
		}
	}

	return ResponseType;
}

void UMMOARPGDbServerObject::GetSerialTArray(TCHAR* InPrefix, const TArray<FString>& InTArray, FString& OutString)
{
	for (auto& Element : InTArray)
	{
		OutString += Element + InPrefix;
	}
	OutString.RemoveFromEnd(InPrefix);
}

bool UMMOARPGDbServerObject::GetUserData(int32 InUserID, FString& OutJson)
{
	FMMOARPGUserData UserData;
	UserData.ID = InUserID;

	FString SQL = FString::Printf(TEXT("SELECT user_login, user_email, user_url, display_name FROM wp_users WHERE ID=%i;"), InUserID);
	TArray<FSimpleMysqlResult> Results;
	if (Get(SQL, Results))
	{
		if (Results.Num() > 0)
		{
			for (auto& Result : Results)
			{
				if (FString* UserLogin = Result.Rows.Find(TEXT("user_login")))
				{
					UserData.Account = *UserLogin;
				}
				if (FString* UserEmail = Result.Rows.Find(TEXT("user_email")))
				{
					UserData.Email = *UserEmail;
				}
				//if (FString* UserUrl = Result.Rows.Find(TEXT("user_url")))
				//{
				//	UserData.Url = UserUrl
				//}
				if (FString* UserDisplayName = Result.Rows.Find(TEXT("display_name")))
				{
					UserData.NickName = *UserDisplayName;
				}
				// TODO: AvatarURL
			}
		}
	}

	// serialize UserData to Json
	NetDataParser::UserDataToJson(UserData, OutJson);

	return OutJson.Len() > 0;
}

bool UMMOARPGDbServerObject::GetCAwithSlot(int32 InUserID, int32 InSlotPos, FString& OutJson)
{
	/* Step1: Get Character IDs metadata */
	TArray<FString> CAIDs;

	if (GetCAIDs(InUserID, CAIDs))
	{
		FString CAIDsString;
		GetSerialTArray(TEXT(","), CAIDs, CAIDsString);

		/* Step2: Get Character Appearances by IDs */
		FString GetCASQL = FString::Printf(TEXT("SELECT * FROM mmoarpg_characters_ca WHERE id IN (%s) and mmoarpg_slot=%i;"),
			*CAIDsString, InSlotPos);
		TArray<FSimpleMysqlResult> GetCASQLResults;
		if (Get(GetCASQL, GetCASQLResults))
		{
			// get character appearances
			if (GetCASQLResults.Num() > 0)
			{
				FMMOARPGCharacterAppearance CA;

				for (auto& GetCASQLResult : GetCASQLResults)
				{
					if (FString* Name = GetCASQLResult.Rows.Find(TEXT("mmoarpg_name")))
					{
						CA.Name = *Name;
					}
					if (FString* Date = GetCASQLResult.Rows.Find(TEXT("mmoarpg_date")))
					{
						CA.CreationDate = *Date;
					}
					if (FString* Slot = GetCASQLResult.Rows.Find(TEXT("mmoarpg_slot")))
					{
						CA.SlotPos = FCString::Atoi(**Slot);
					}
					if (FString* LegSize = GetCASQLResult.Rows.Find(TEXT("leg_size")))
					{
						CA.LegSize = FCString::Atof(**LegSize);
					}
					if (FString* WaistSize = GetCASQLResult.Rows.Find(TEXT("waist_size")))
					{
						CA.WaistSize = FCString::Atof(**WaistSize);
					}
					if (FString* ArmSize = GetCASQLResult.Rows.Find(TEXT("arm_size")))
					{
						CA.ArmSize = FCString::Atof(**ArmSize);
					}

					NetDataParser::CharacterAppearanceToJson(CA, OutJson);

					return !OutJson.IsEmpty();
				}
			}
		}
	}

	return false;
}

bool UMMOARPGDbServerObject::GetCAIDs(int32 InUserID, TArray<FString>& OutCAIDs)
{
	FString GetCAIDsSQL = FString::Printf(TEXT("SELECT meta_value FROM wp_usermeta WHERE user_id = %i and meta_key = 'character_ca_id';"), InUserID);

	TArray<FSimpleMysqlResult> GetCAIDsResults;
	if (Get(GetCAIDsSQL, GetCAIDsResults))
	{
		// get character slots
		if (GetCAIDsResults.Num() > 0)
		{
			for (auto& GetCAIDsResult : GetCAIDsResults)
			{
				if (FString* CAIDsString = GetCAIDsResult.Rows.Find(TEXT("meta_value")))
				{
					CAIDsString->ParseIntoArray(OutCAIDs, TEXT("|"));
				}
			}
		}

		return true;
	}

	//return OutCAIDs.Num() > 0;
	return false;
}

bool UMMOARPGDbServerObject::Post(const FString& InSQL)
{
	if (!InSQL.IsEmpty())
	{
		if (MysqlObjectWrite)
		{
			FString ErrMsg;
			USimpleMySQLLibrary::QueryLink(MysqlObjectWrite, InSQL, ErrMsg);

			if (ErrMsg.IsEmpty())
			{
				return true;
			}
			else
			{
				UE_LOG(LogMMOARPGDbServer, Error, TEXT("MMO DB Server Error: Post msg [%s]"), *ErrMsg);
			}
		}
	}

	return false;
}

bool UMMOARPGDbServerObject::Get(const FString& InSQL, TArray<FSimpleMysqlResult>& Results)
{
	if (!InSQL.IsEmpty())
	{
		if (MysqlObjectWrite)
		{
			FSimpleMysqlDebugResult Debug;
			Debug.bPrintToLog = true;

			FString ErrMsg;
			USimpleMySQLLibrary::QueryLinkResult(
				MysqlObjectWrite, 
				InSQL, 
				Results, 
				ErrMsg, 
				EMysqlQuerySaveType::STORE_RESULT, 
				Debug
			);

			if (ErrMsg.IsEmpty())
			{
				return true;
			}
			else
			{
				UE_LOG(LogMMOARPGDbServer, Error, TEXT("MMO DB Server Error: Get msg [%s]"), *ErrMsg);
			}
		}
	}

	return false;
}
