// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModelingOperators.h" //IDynamicMeshOperatorFactory
#include "InteractiveTool.h" //UInteractiveToolPropertySet
#include "InteractiveToolBuilder.h" //UInteractiveToolBuilder
#include "InteractiveToolChange.h" //FToolCommandChange
#include "SingleSelectionTool.h" //USingleSelectionTool
#include "Operations/FFDLattice.h"
#include "LatticeDeformerTool.generated.h"

class ULatticeControlPointsMechanic;
class UMeshOpPreviewWithBackgroundCompute;

UCLASS()
class MESHMODELINGTOOLS_API ULatticeDeformerToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


UENUM()
enum class ELatticeInterpolationType : uint8
{
	/** Use trilinear interpolation to get new mesh vertex positions from the lattice */
	Linear UMETA(DisplayName = "Linear"),

	/** Use tricubic interpolation to get new mesh vertex positions from the lattice */
	Cubic UMETA(DisplayName = "Cubic")
};


UCLASS()
class MESHMODELINGTOOLS_API ULatticeDeformerToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:

	/** Number of lattice vertices along the X axis */
	UPROPERTY(EditAnywhere, Category = Resolution, meta = (UIMin = "2", ClampMin = "2", EditCondition = "bCanChangeResolution", HideEditConditionToggle))
	int XAxisResolution = 5;

	/** Number of lattice vertices along the Y axis */
	UPROPERTY(EditAnywhere, Category = Resolution, meta = (UIMin = "2", ClampMin = "2", EditCondition = "bCanChangeResolution", HideEditConditionToggle))
	int YAxisResolution = 5;

	/** Number of lattice vertices along the Z axis */
	UPROPERTY(EditAnywhere, Category = Resolution, meta = (UIMin = "2", ClampMin = "2", EditCondition = "bCanChangeResolution", HideEditConditionToggle))
	int ZAxisResolution = 5;

	/** Relative distance the lattice extends from the mesh */
	UPROPERTY(EditAnywhere, Category = Resolution, meta = (UIMin = "0.01", ClampMin = "0.01", UIMax = "2", ClampMax = "5", EditCondition = "bCanChangeResolution", HideEditConditionToggle))
	float Padding = 0.01;

	/** Whether to use linear or cubic interpolation to get new mesh vertex positions from the lattice */
	UPROPERTY(EditAnywhere, Category = Interpolation )
	ELatticeInterpolationType InterpolationType = ELatticeInterpolationType::Linear;

	// Not user visible - used to disallow changing the lattice resolution after deformation
	UPROPERTY(meta = (TransientToolProperty))
	bool bCanChangeResolution = true;

	/** Whether the gizmo's axes remain aligned with world axes or rotate as the gizmo is transformed */
	UPROPERTY(EditAnywhere, Category = Gizmo)
	EToolContextCoordinateSystem GizmoCoordinateSystem = EToolContextCoordinateSystem::Local;

	/** If Set Pivot Mode is active, the gizmo can be repositioned without moving the selected lattice points */
	UPROPERTY(EditAnywhere, Category = Gizmo)
	bool bSetPivotMode = false;

};


UCLASS()
class MESHMODELINGTOOLS_API ULatticeDeformerOperatorFactory : public UObject, public IDynamicMeshOperatorFactory
{
	GENERATED_BODY()

public:
	// IDynamicMeshOperatorFactory API
	virtual TUniquePtr<FDynamicMeshOperator> MakeNewOperator() override;

	UPROPERTY()
	ULatticeDeformerTool* LatticeDeformerTool;
};


/** Deform a mesh using a regular hexahedral lattice */
UCLASS()
class MESHMODELINGTOOLS_API ULatticeDeformerTool : public USingleSelectionTool
{
	GENERATED_BODY()

public:

	virtual void DrawHUD(FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI) override;

	virtual void SetWorld(UWorld* World) { TargetWorld = World; }
	virtual void SetAssetAPI(IToolsContextAssetAPI* NewAssetApi) { AssetAPI = NewAssetApi; }

	virtual bool HasCancel() const override { return true; }
	virtual bool HasAccept() const override { return true; }
	virtual bool CanAccept() const override;

	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	virtual void OnTick(float DeltaTime) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

	FVector3i GetLatticeResolution() const;

protected:

	UWorld* TargetWorld;
	IToolsContextAssetAPI* AssetAPI;

	// Input mesh
	TSharedPtr<FDynamicMesh3, ESPMode::ThreadSafe> OriginalMesh;

	TSharedPtr<FFFDLattice, ESPMode::ThreadSafe> Lattice;

	UPROPERTY()
	ULatticeControlPointsMechanic* ControlPointsMechanic = nullptr;

	UPROPERTY()
	ULatticeDeformerToolProperties* Settings = nullptr;

	UPROPERTY()
	UMeshOpPreviewWithBackgroundCompute* Preview = nullptr;

	UPROPERTY()
	bool bLatticeDeformed = false;

	bool bShouldRebuild = false;

	// Create and store an FFFDLattice. Pass out the lattice's positions and edges.
	void InitializeLattice(TArray<FVector3d>& OutLatticePoints, TArray<FVector2i>& OutLatticeEdges);

	void StartPreview();

	friend class ULatticeDeformerOperatorFactory;
};
