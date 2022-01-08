// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Tools/BaseAssetToolkit.h"
#include "Delegates/IDelegateInstance.h"

class FToolsContextQueriesImpl;
class FToolsContextTransactionImpl;
class SEditorViewport;
class FEditorViewportClient;
class UAssetEditor;
class UInteractiveToolsContext;

class FExampleAssetToolkit : public FBaseAssetToolkit
{
public:
	FExampleAssetToolkit(UAssetEditor* InOwningAssetEditor, UInteractiveToolsContext* InContext);
	virtual ~FExampleAssetToolkit();


protected:
	// Base Asset Toolkit overrides
	virtual AssetEditorViewportFactoryFunction GetViewportDelegate() override;
	virtual TSharedPtr<FEditorViewportClient> CreateEditorViewportClient() const override;
	virtual void PostInitAssetEditor() override;
	// End Base Asset Toolkit overrides

	void AddInputBehaviorsForEditorClientViewport(TSharedPtr<FEditorViewportClient>& InViewportClient) const;

	UInteractiveToolsContext* ToolsContext;
	TSharedPtr<FToolsContextQueriesImpl> ToolsContextQueries;
	TSharedPtr<FToolsContextTransactionImpl> ToolsContextTransactions;
};
