#include "MMOARPGDbServerObject.h"

#include "Blueprint/SimpleMysqlObject.h" // Plugin: SimpleMySQL
#include "SimpleMySQLibrary.h" // Plugin: SimpleMySQL
#include "Global/SimpleNetGlobalInfo.h" // Plugin: SimpleNetChannel
#include "Protocol/LoginProtocol.h" // Plugin: MMOARPGComm
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

		// TODO: send back user data
		FString UserDataBack = TEXT("{}");

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
				SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);
			}
		}
		else
		{
			ELoginType ResponseType = ELoginType::DB_ERROR;
			SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);
		}		
		break;
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
			FString UserDataBack = TEXT("{}");

			if (PV == VERIFICATION_SUCCESS)
			{
				// Password Verification is Success
				if (UserID != 0)
				{
					// TODO: Send back user info
					FString SQL = FString::Printf(TEXT("SELECT user_login, user_email, user_url, display_name FROM wp_users WHERE ID=%i;"), UserID);
					TArray<FSimpleMysqlResult> Results;
					if (Get(SQL, Results))
					{
						if (Results.Num() > 0)
						{
							for (auto& Result : Results)
							{
								if (FString* UserLogin = Result.Rows.Find(TEXT("user_login")))
								{

								}
								if (FString* UserEmail = Result.Rows.Find(TEXT("user_email")))
								{

								}
								if (FString* UserUrl = Result.Rows.Find(TEXT("user_url")))
								{

								}
								if (FString* UserDisplayName = Result.Rows.Find(TEXT("display_name")))
								{

								}
							}
						}
					}

					ELoginType ResponseType = ELoginType::LOGIN_SUCCESS;
					SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);
				}
			}
			else
			{
				// Password Verification is Fail
				ELoginType ResponseType = ELoginType::LOGIN_PASSWORD_ERROR;
				SIMPLE_PROTOCOLS_SEND(SP_LoginResponses, AddrInfo, ResponseType, UserDataBack);
			}
		}
	}
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
