// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Casts.h"

#include "MetasoundArrayNodesRegistration.h"
#include "MetasoundAutoConverterNode.h"
#include "MetasoundConverterNodeRegistrationMacro.h"
#include "MetasoundDataFactory.h"
#include "MetasoundDataReference.h"
#include "MetasoundEnum.h"
#include "MetasoundFrontendRegistries.h"
#include "MetasoundInputNode.h"
#include "MetasoundLiteral.h"
#include "MetasoundLog.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundOperatorInterface.h"
#include "MetasoundOutputNode.h"
#include "MetasoundPrimitives.h"
#include "MetasoundReceiveNode.h"
#include "MetasoundRouter.h"
#include "MetasoundSendNode.h"
#include "MetasoundTransmissionRegistration.h"

#include <type_traits>

namespace Metasound
{
	/** Enables or disables automatic registration of array types given a 
	 * MetaSound data type. By default this is true, and all data types will have
	 * an associated TArray<DataType> registered if it is supported. */
	template<typename ... Type>
	struct TEnableAutoArrayTypeRegistration
	{
		static constexpr bool Value = true;
	};

	/** Enables or disables automatic registration of auto conversion nodes given a 
	 * MetaSound data type. By default this is true, and all data types will have
	 * associated conversion nodes registered based upon the data types supported
	 * constructors and implicit conversions. */
	template<typename ... Type>
	struct TEnableAutoConverterNodeRegistration
	{
		static constexpr bool Value = true;
	};

	/** Enables or disables send and receive node registration for a given MetaSound
	 * data type. By default this is true and all data types supported by the transmission
	 * system will have associated send and receive nodes. */
	template<typename ... Type>
	struct TEnableTransmissionNodeRegistration
	{
		static constexpr bool Value = true;
	};

	namespace MetasoundDataTypeRegistrationPrivate
	{

		// Helper utility to test if we can transmit a datatype between a send and a receive node.
		template <typename TDataType>
		struct TIsTransmittable
		{
		private:
			static constexpr bool bIsCopyConstructible = std::is_copy_constructible<TDataType>::value;
			static constexpr bool bIsCopyAssignable = std::is_copy_assignable<TDataType>::value;

			// TODO: audio types were intended to be send/receive nodes but they require 
			// template specialization.  TIsTransmittable should ask the Send and Receive nodes
			// if they are transmittable rather than attempting to do the logic for all
			// types here. 
			//static constexpr bool bIsAudioDataType = TIsDerivedFrom<TDataType, IAudioDataType>::Value;

			static constexpr bool bCanBeTransmitted = bIsCopyConstructible && bIsCopyAssignable;

		public:

			static constexpr bool Value = bCanBeTransmitted;
		};

		// Specialization of TIsTransmittable<> for TArray to handle lax TArray copy constructor 
		// definition. This can be removed once updates to TArray are merged into the same codebase.
		// TODO: delete me eventually.
		template<typename TElementType>
		struct TIsTransmittable<TArray<TElementType>>
		{
			// Depend on copy constructor of elements to determine whether TArray is copy constructible
			// and copy assignable. Definitions for TArray copy construction and copy assignment are
			// defined by default whether or not the TArray element types support them. This logic
			// skips checking the TArray object itselt and instead checks the TArray element type
			// directly.

			static constexpr bool Value = TIsTransmittable<TElementType>::Value;
		};

		// Determines whether an auto converter node will be registered to convert 
		// between two types. 
		template<typename TFromDataType, typename TToDataType>
		struct TIsAutoConvertible
		{
			static constexpr bool Value = std::is_convertible<TFromDataType, TToDataType>::value;
		};
		// Returns the Array version of a literal type if it exists.
		template<ELiteralType LiteralType>
		struct TLiteralArrayEnum 
		{
			// Default to TArray default constructor by using
			// ELiteralType::None
			static constexpr ELiteralType Value = ELiteralType::None;
		};

		// Specialization for None->NoneArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::None>
		{
			static constexpr ELiteralType Value = ELiteralType::NoneArray;
		};
		 
		// Specialization for Boolean->BooleanArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::Boolean>
		{
			static constexpr ELiteralType Value = ELiteralType::BooleanArray;
		};
		
		// Specialization for Integer->IntegerArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::Integer>
		{
			static constexpr ELiteralType Value = ELiteralType::IntegerArray;
		};
		
		// Specialization for Float->FloatArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::Float>
		{
			static constexpr ELiteralType Value = ELiteralType::FloatArray;
		};
		
		// Specialization for String->StringArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::String>
		{
			static constexpr ELiteralType Value = ELiteralType::StringArray;
		};
		
		// Specialization for UObjectProxy->UObjectProxyArray
		template<>
		struct TLiteralArrayEnum<ELiteralType::UObjectProxy>
		{
			static constexpr ELiteralType Value = ELiteralType::UObjectProxyArray;
		};

		// SFINAE used to optionally invoke subclasses of IAudioProxyDataFactory when we can.
		template<typename UClassToUse, typename TEnableIf<TIsDerivedFrom<UClassToUse, IAudioProxyDataFactory>::Value, bool>::Type = true>
		IAudioProxyDataFactory* CastToAudioProxyDataFactory(UObject* InObject)
		{
			if (InObject)
			{
				UClassToUse* DowncastObject = Cast<UClassToUse>(InObject);
				if (ensureAlways(DowncastObject))
				{
					return static_cast<IAudioProxyDataFactory*>(DowncastObject);
				}
			}

			return nullptr;
		}

		template<typename UClassToUse, typename TEnableIf<!TIsDerivedFrom<UClassToUse, IAudioProxyDataFactory>::Value, bool>::Type = true>
		IAudioProxyDataFactory* CastToAudioProxyDataFactory(UObject* InObject)
		{
			return nullptr;
		}
		

		// This utility function can be used to optionally check to see if we can transmit a data type, and autogenerate send and receive nodes for that datatype.
		template<typename TDataType, typename TEnableIf<TIsTransmittable<TDataType>::Value, bool>::Type = true>
		void AttemptToRegisterSendAndReceiveNodes()
		{
			if (TEnableTransmissionNodeRegistration<TDataType>::Value)
			{
				ensureAlways(RegisterNodeWithFrontend<Metasound::TSendNode<TDataType>>());
				ensureAlways(RegisterNodeWithFrontend<Metasound::TReceiveNode<TDataType>>());
			}
		}

		template<typename TDataType, typename TEnableIf<!TIsTransmittable<TDataType>::Value, bool>::Type = true>
		void AttemptToRegisterSendAndReceiveNodes()
		{
			// This implementation intentionally noops, because Metasound::TIsTransmittable is false for this datatype.
			// This is either because the datatype is not trivially copyable, and thus can't be buffered between threads,
			// or it's not an audio buffer type, which we use Audio::FPatchMixerSplitter instances for.
		}

		// This utility function can be used to check to see if we can static cast between two types, and autogenerate a node for that static cast.
		template<typename TFromDataType, typename TToDataType, typename std::enable_if<TIsAutoConvertible<TFromDataType, TToDataType>::Value, bool>::type = true>
		void AttemptToRegisterConverter()
		{
			using FConverterNode = Metasound::TAutoConverterNode<TFromDataType, TToDataType>;

			if (TEnableAutoConverterNodeRegistration<TFromDataType, TToDataType>::Value)
			{
				const FNodeClassMetadata& Metadata = FConverterNode::GetAutoConverterNodeMetadata();
				const Metasound::Frontend::FNodeRegistryKey Key = FMetasoundFrontendRegistryContainer::GetRegistryKey(Metadata);

				if (!std::is_same<TFromDataType, TToDataType>::value && !FMetasoundFrontendRegistryContainer::Get()->IsNodeRegistered(Key))
				{
					ensureAlways(RegisterNodeWithFrontend<FConverterNode>(Metadata));
					
					bool bSucessfullyRegisteredConversionNode = RegisterConversionNode<FConverterNode, TFromDataType, TToDataType>(FConverterNode::GetInputName(), FConverterNode::GetOutputName(), Metadata);
					ensureAlways(bSucessfullyRegisteredConversionNode);
				}
			}
		}

		template<typename TFromDataType, typename TToDataType, typename std::enable_if<!TIsAutoConvertible<TFromDataType, TToDataType>::Value, int>::type = 0>
		void AttemptToRegisterConverter()
		{
			// This implementation intentionally noops, because static_cast<TFromDataType>(TToDataType&) is invalid.
		}

		// Here we attempt to infer and autogenerate conversions for basic datatypes.
		template<typename TDataType>
		void RegisterConverterNodes()
		{
			// Conversions to this data type:
			AttemptToRegisterConverter<bool, TDataType>();
			AttemptToRegisterConverter<int32, TDataType>();
			AttemptToRegisterConverter<float, TDataType>();
			AttemptToRegisterConverter<FString, TDataType>();

			// Conversions from this data type:
			AttemptToRegisterConverter<TDataType, bool>();
			AttemptToRegisterConverter<TDataType, int32>();
			AttemptToRegisterConverter<TDataType, float>();
			AttemptToRegisterConverter<TDataType, FString>();
		}

		// SFINAE for enum types.
		template<typename TDataType, typename std::enable_if<TEnumTraits<TDataType>::bIsEnum, bool>::type = true>
		bool RegisterEnumDataTypeWithFrontend()
		{
			using InnerType = typename TEnumTraits<TDataType>::InnerType;
			using FStringHelper = TEnumStringHelper<InnerType>;

			struct FEnumHandler : Metasound::Frontend::IEnumDataTypeInterface
			{
				FName GetNamespace() const override
				{
					return FStringHelper::GetNamespace();
				}
				int32 GetDefaultValue() const override
				{
					return static_cast<int32>(TEnumTraits<TDataType>::DefaultValue);
				}
				const TArray<FGenericInt32Entry>& GetAllEntries() const override
				{
					auto BuildIntEntries = []()
					{
						// Convert to int32 representation 
						TArray<FGenericInt32Entry> IntEntries;
						IntEntries.Reserve(FStringHelper::GetAllEntries().Num());
						for (const TEnumEntry<InnerType>& i : FStringHelper::GetAllEntries())
						{
							IntEntries.Emplace(i);
						}
						return IntEntries;
					};
					static const TArray<FGenericInt32Entry> IntEntries = BuildIntEntries();
					return IntEntries;
				}
			};

			return FMetasoundFrontendRegistryContainer::Get()->RegisterEnumDataInterface(
				GetMetasoundDataTypeName<TDataType>(), MakeShared<FEnumHandler>());
		}
		
		// SFINAE stub for non-enum types. 
		template<typename TDataType, typename std::enable_if<!TEnumTraits<TDataType>::bIsEnum, bool>::type = true>
		bool RegisterEnumDataTypeWithFrontend() { return false; }

		template<typename TDataType, ELiteralType PreferredArgType = ELiteralType::None, typename UClassToUse = UObject>
		bool RegisterDataTypeWithFrontendInternal()
		{
			// if we reenter this code (because DECLARE_METASOUND_DATA_REFERENCE_TYPES was called twice with the same type),
			// we catch it here.
			static bool bAlreadyRegisteredThisDataType = false;
			if (bAlreadyRegisteredThisDataType)
			{
				UE_LOG(LogMetaSound, Display, TEXT("Tried to call REGISTER_METASOUND_DATATYPE twice with the same class %s. ignoring the second call. Likely because REGISTER_METASOUND_DATATYPE is in a header that's used in multiple modules. Consider moving it to a private header or cpp file."), TDataReferenceTypeInfo<TDataType>::TypeName)
				return false;
			}

			bAlreadyRegisteredThisDataType = true;

			// Lambdas that generate our template-instantiated input and output nodes:
			FCreateInputNodeFunction InputNodeConstructor = [](FInputNodeConstructorParams&& InParams) -> TUniquePtr<INode>
			{
				return TUniquePtr<INode>(new TInputNode<TDataType>(InParams.InNodeName, InParams.InInstanceID, InParams.InVertexName, MoveTemp(InParams.InitParam)));
			};

			FCreateMetasoundFrontendClassFunction CreateFrontendInputClass = []() -> FMetasoundFrontendClass
			{
				// Create class info using prototype node
				// TODO: register input nodes with static class info.
				static TInputNode<TDataType> Prototype(TEXT(""), FGuid(), TEXT(""), FLiteral());
				return Metasound::Frontend::GenerateClassDescription(Prototype.GetMetadata(), EMetasoundFrontendClassType::Input);
			};

			FCreateOutputNodeFunction OutputNodeConstructor = [](const FOutputNodeConstructorParams& InParams) -> TUniquePtr<INode>
			{
				return TUniquePtr<INode>(new TOutputNode<TDataType>(InParams.InNodeName, InParams.InInstanceID, InParams.InVertexName));
			};

			FCreateMetasoundFrontendClassFunction CreateFrontendOutputClass = []() -> FMetasoundFrontendClass
			{
				// Create class info using prototype node
				// TODO: register input nodes with static class info.
				static TOutputNode<TDataType> Prototype(TEXT(""), FGuid(), TEXT(""));
				return Metasound::Frontend::GenerateClassDescription(Prototype.GetMetadata(), EMetasoundFrontendClassType::Output);
			};
			
			// By default, this function should not be used, unless the preferred arg type is UObjectProxy or UObjectProxyArray, and UClassToUse should be specified.
			FCreateAudioProxyFunction ProxyGenerator = [](UObject* InObject) -> Audio::IProxyDataPtr
			{
				checkNoEntry();
				return Audio::IProxyDataPtr(nullptr);
			};

			// If this datatype uses a UObject or UObject array literal, we generate a lambda to build a proxy here:
			constexpr bool bSpecifiedUClassForProxy = !TIsSame<UClassToUse, UObject>::Value;
			if (bSpecifiedUClassForProxy)
			{
				static_assert(!bSpecifiedUClassForProxy || std::is_base_of<IAudioProxyDataFactory, UClassToUse>::value, "If a Metasound Datatype uses a UObject as a literal, the UClass of that object needs to also derive from Audio::IProxyDataFactory. See USoundWave as an example.");
				ProxyGenerator = [](UObject* InObject) -> Audio::IProxyDataPtr
				{
					if (InObject)
					{
						IAudioProxyDataFactory* ObjectAsFactory = CastToAudioProxyDataFactory<UClassToUse>(InObject);
						if (ensureAlways(ObjectAsFactory))
						{
							static FName ProxySubsystemName = TEXT("Metasound");

							Audio::FProxyDataInitParams ProxyInitParams;
							ProxyInitParams.NameOfFeatureRequestingProxy = ProxySubsystemName;

							return ObjectAsFactory->CreateNewProxyData(ProxyInitParams);
						}
					}

					return Audio::IProxyDataPtr(nullptr);
				};
			}

			FCreateDataChannelFunction CreateDataChannelFunc(&FTransmissionDataChannelFactory::CreateDataChannel<TDataType>);

			// Pack all of our various constructor lambdas to a single struct.
			FDataTypeConstructorCallbacks Callbacks = 
			{ 
				MoveTemp(InputNodeConstructor), 
				MoveTemp(CreateFrontendInputClass),
				MoveTemp(OutputNodeConstructor), 
				MoveTemp(CreateFrontendOutputClass),
				MoveTemp(ProxyGenerator),
				MoveTemp(CreateDataChannelFunc)
			};


			FDataTypeRegistryInfo RegistryInfo;

			RegistryInfo.DataTypeName = GetMetasoundDataTypeName<TDataType>();
			RegistryInfo.PreferredLiteralType = PreferredArgType;

			RegistryInfo.bIsDefaultParsable = TIsParsable<TDataType, FLiteral::FNone>::Value;		
			RegistryInfo.bIsBoolParsable = TIsParsable<TDataType, bool>::Value;
			RegistryInfo.bIsIntParsable = TIsParsable<TDataType, int32>::Value;
			RegistryInfo.bIsFloatParsable = TIsParsable<TDataType, float>::Value;
			RegistryInfo.bIsStringParsable = TIsParsable<TDataType, FString>::Value;
			RegistryInfo.bIsProxyParsable = TIsParsable<TDataType, const Audio::IProxyDataPtr&>::Value;

			RegistryInfo.bIsDefaultArrayParsable = TIsParsable<TDataType, TArray<FLiteral::FNone>>::Value;		
			RegistryInfo.bIsBoolArrayParsable = TIsParsable<TDataType, TArray<bool>>::Value;
			RegistryInfo.bIsIntArrayParsable = TIsParsable<TDataType, TArray<int32>>::Value;
			RegistryInfo.bIsFloatArrayParsable = TIsParsable<TDataType, TArray<float>>::Value;
			RegistryInfo.bIsStringArrayParsable = TIsParsable<TDataType, TArray<FString>>::Value;

			RegistryInfo.bIsProxyArrayParsable = TIsParsable<TDataType, const TArray<Audio::IProxyDataPtr>& >::Value;
			RegistryInfo.bIsEnum = TEnumTraits<TDataType>::bIsEnum;

			RegistryInfo.bIsTransmittable = TIsTransmittable<TDataType>::Value;
			
			RegistryInfo.ProxyGeneratorClass = UClassToUse::StaticClass();

			bool bSucceeded = FMetasoundFrontendRegistryContainer::Get()->RegisterDataType(RegistryInfo, MoveTemp(Callbacks));
			ensureAlwaysMsgf(bSucceeded, TEXT("Failed to register data type %s in the node registry!"), *GetMetasoundDataTypeString<TDataType>());

			// If its an enum, register its data interface AFTER we've registered the data type.
			if (TEnumTraits<TDataType>::bIsEnum)
			{
				if (!ensure(RegisterEnumDataTypeWithFrontend<TDataType>()))
				{
					return false;
				}
			}

			RegisterConverterNodes<TDataType>();
			AttemptToRegisterSendAndReceiveNodes<TDataType>();
			
			return bSucceeded;
		}

		template<typename TDataType, ELiteralType PreferredArgType, typename UClassToUse>
		bool RegisterDataTypeArrayWithFrontend()
		{
			using namespace MetasoundDataTypeRegistrationPrivate;
			using TArrayType = TArray<TDataType>;

			if (TEnableAutoArrayTypeRegistration<TDataType>::Value)
			{
				bool bSuccess = RegisterDataTypeWithFrontendInternal<TArrayType, TLiteralArrayEnum<PreferredArgType>::Value, UClassToUse>();
				bSuccess = bSuccess && RegisterArrayNodes<TArrayType>();
				return bSuccess;
			}

			return true;
		}
	}
	
	template<typename TDataType, ELiteralType PreferredArgType = ELiteralType::None, typename UClassToUse = UObject>
	bool RegisterDataTypeWithFrontend()
	{
		using namespace MetasoundDataTypeRegistrationPrivate;

		// Register TDataType as a metasound data type.
		bool bSuccess = RegisterDataTypeWithFrontendInternal<TDataType, PreferredArgType, UClassToUse>();
		ensure(bSuccess);

		// Register TArray<TDataType> as a metasound data type.
		bSuccess = bSuccess && RegisterDataTypeArrayWithFrontend<TDataType, PreferredArgType, UClassToUse>();
		ensure(bSuccess);

		return bSuccess;
	}


	template<typename DataType>
	struct TMetasoundDataTypeRegistration
	{
		static_assert(std::is_same<DataType, typename std::decay<DataType>::type>::value, "DataType and decayed DataType must be the same");
		
		static constexpr bool bCanRegister = TInputNode<DataType>::bCanRegister;

		static const bool bSuccessfullyRegistered;
	};
}

// This should be used to expose a datatype as a potential input or output for a metasound graph.
// The first argument to the macro is the class to expose.
// the second argument is the display name of that type in the Metasound editor.
// Optionally, a Metasound::ELiteralType can be passed in to designate a preferred literal type-
// For example, if Metasound::ELiteralType::Float is passed in, we will default to using a float parameter to create this datatype.
// If no argument is passed in, we will infer a literal type to use.
// If 
// Metasound::ELiteralType::Invalid can be used to enforce that we don't provide space for a literal, in which case you should have a default constructor or a constructor that takes [const FOperatorSettings&] implemented.
// If you pass in a preferred arg type, please make sure that the passed in datatype has a matching constructor, since we won't check this until runtime.

#define CANNOT_REGISTER_METASOUND_DATA_TYPE_ASSERT_STRING(DataType) \
"To register " #DataType " to be used as a Metasounds input or output type, it needs a default constructor or one of the following constructors must be implemented:  " \
#DataType "(), " \
#DataType "(bool InValue), " \
#DataType "(int32 InValue), " \
#DataType "(float InValue), " \
#DataType "(const FString& InString)" \
#DataType "(const Audio::IProxyDataPtr& InData),  or " \
#DataType "(const TArray<Audio::IProxyDataPtr>& InProxyArray)."\
#DataType "(const ::Metasound::FOperatorSettings& InSettings), " \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, bool InValue), " \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, int32 InValue), " \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, float InValue), " \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, const FString& InString)" \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, const Audio::IProxyDataPtr& InData),  or " \
#DataType "(const ::Metasound::FOperatorSettings& InSettings, const TArray<Audio::IProxyDataPtr>& InProxyArray)."

#define REGISTER_METASOUND_DATATYPE(DataType, DataTypeName, ...) \
	DEFINE_METASOUND_DATA_TYPE(DataType, DataTypeName); \
	static_assert(::Metasound::TMetasoundDataTypeRegistration<DataType>::bCanRegister, CANNOT_REGISTER_METASOUND_DATA_TYPE_ASSERT_STRING(DataType)); \
	template<> const bool ::Metasound::TMetasoundDataTypeRegistration<DataType>::bSuccessfullyRegistered = ::FMetasoundFrontendRegistryContainer::Get()->EnqueueInitCommand([](){ ::Metasound::RegisterDataTypeWithFrontend<DataType, ##__VA_ARGS__>(); }); // This static bool is useful for debugging, but also is the only way the compiler will let us call this function outside of an expression.

