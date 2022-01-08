// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetasoundFrontendController.h"

#include "MetasoundFrontendBaseClasses.h"
#include "MetasoundFrontendInvalidController.h"
#include "MetasoundFrontendStandardController.h"

namespace Metasound
{
	namespace Frontend
	{
		FOutputHandle IOutputController::GetInvalidHandle()
		{
			return FInvalidOutputController::GetInvalid();
		}

		FInputHandle IInputController::GetInvalidHandle()
		{
			return FInvalidInputController::GetInvalid();
		}

		FNodeHandle INodeController::GetInvalidHandle()
		{
			return FInvalidNodeController::GetInvalid();
		}

		FGraphHandle IGraphController::GetInvalidHandle()
		{
			return FInvalidGraphController::GetInvalid();
		}

		FDocumentHandle IDocumentController::GetInvalidHandle()
		{
			return FInvalidDocumentController::GetInvalid();
		}
			
		FDocumentHandle IDocumentController::CreateDocumentHandle(TAccessPtr<FMetasoundFrontendDocument> InDocument)
		{
			// Create using standard document controller.
			return FDocumentController::CreateDocumentHandle(InDocument);
		}

		FConstDocumentHandle IDocumentController::CreateDocumentHandle(TAccessPtr<const FMetasoundFrontendDocument> InDocument)
		{
			// Create using standard document controller. 
			return FDocumentController::CreateDocumentHandle(ConstCastAccessPtr<FMetasoundFrontendDocument>(InDocument));
		}
	}
}
