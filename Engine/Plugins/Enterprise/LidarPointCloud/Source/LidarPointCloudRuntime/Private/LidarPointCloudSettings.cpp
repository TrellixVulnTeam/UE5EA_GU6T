// Copyright Epic Games, Inc. All Rights Reserved.

#include "LidarPointCloudSettings.h"
#include "Math/UnrealMathUtility.h"

ULidarPointCloudSettings::ULidarPointCloudSettings()
	: DuplicateHandling(ELidarPointCloudDuplicateHandling::SelectBrighter)
	, MaxDistanceForDuplicate(KINDA_SMALL_NUMBER)
	, MaxBucketSize(200)
	, NodeGridResolution(96)
	, MultithreadingInsertionBatchSize(500000)
	, bUseAsyncImport(true)
	, bPrioritizeActiveViewport(true)
	, CachedNodeLifetime(5.0f)
	, bUseRenderDataSmoothing(true)
	, RenderDataSmoothingMaxFrametime(0.01f)
	, MeshingBatchSize(128)
	, bAutoCenterOnImport(true)
	, ImportScale(100)
	, ExportScale(0.01f)
{
}
