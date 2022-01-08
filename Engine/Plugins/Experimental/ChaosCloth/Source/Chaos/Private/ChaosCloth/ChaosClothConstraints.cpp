// Copyright Epic Games, Inc. All Rights Reserved.
#include "ChaosCloth/ChaosClothConstraints.h"
#include "Chaos/PBDSpringConstraints.h"
#include "Chaos/XPBDSpringConstraints.h"
#include "Chaos/PBDBendingConstraints.h"
#include "Chaos/PBDAxialSpringConstraints.h"
#include "Chaos/XPBDAxialSpringConstraints.h"
#include "Chaos/PBDVolumeConstraint.h"
#include "Chaos/XPBDLongRangeConstraints.h"
#include "Chaos/PBDSphericalConstraint.h"
#include "Chaos/PBDAnimDriveConstraint.h"
#include "Chaos/PBDShapeConstraints.h"
#include "Chaos/PBDCollisionSpringConstraints.h"
#include "Chaos/PBDEvolution.h"

using namespace Chaos;

FClothConstraints::FClothConstraints()
	: Evolution(nullptr)
	, AnimationPositions(nullptr)
	, AnimationNormals(nullptr)
	, ParticleOffset(0)
	, NumParticles(0)
	, ConstraintInitOffset(INDEX_NONE)
	, ConstraintRuleOffset(INDEX_NONE)
	, NumConstraintInits(0)
	, NumConstraintRules(0)
	, MaxDistancesMultiplier(1.f)
	, AnimDriveSpringStiffness(0.f)
{
}

FClothConstraints::~FClothConstraints()
{
}

void FClothConstraints::Initialize(
	TPBDEvolution<float, 3>* InEvolution,
	const TArray<FVec3>& InAnimationPositions,
	const TArray<FVec3>& InAnimationNormals,
	int32 InParticleOffset,
	int32 InNumParticles)
{
	Evolution = InEvolution;
	AnimationPositions = &InAnimationPositions;
	AnimationNormals = &InAnimationNormals;
	ParticleOffset = InParticleOffset;
	NumParticles = InNumParticles;
}

void FClothConstraints::Enable(bool bEnable)
{
	check(Evolution);
	if (ConstraintInitOffset != INDEX_NONE)
	{
		Evolution->ActivateConstraintInitRange(ConstraintInitOffset, bEnable);
	}
	if (ConstraintRuleOffset != INDEX_NONE)
	{
		Evolution->ActivateConstraintRuleRange(ConstraintRuleOffset, bEnable);
	}
}

void FClothConstraints::CreateRules()
{
	check(Evolution);
	check(ConstraintInitOffset == INDEX_NONE)
	if (NumConstraintInits)
	{
		ConstraintInitOffset = Evolution->AddConstraintInitRange(NumConstraintInits, false);
	}
	check(ConstraintRuleOffset == INDEX_NONE)
	if (NumConstraintRules)
	{
		ConstraintRuleOffset = Evolution->AddConstraintRuleRange(NumConstraintRules, false);
	}

	TFunction<void(const TPBDParticles<float, 3>&)>* const ConstraintInits = Evolution->ConstraintInits().GetData() + ConstraintInitOffset;
	TFunction<void(TPBDParticles<float, 3>&, const float)>* const ConstraintRules = Evolution->ConstraintRules().GetData() + ConstraintRuleOffset;

	int32 ConstraintInitIndex = 0;
	int32 ConstraintRuleIndex = 0;

	if (XEdgeConstraints)
	{
		ConstraintInits[ConstraintInitIndex++] =
			[this](const TPBDParticles<float, 3>& /*Particles*/)
			{
				XEdgeConstraints->Init();
			};

		ConstraintRules[ConstraintRuleIndex++] = 
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				XEdgeConstraints->Apply(Particles, Dt);
			};
	}
	if (EdgeConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] = 
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				EdgeConstraints->Apply(Particles, Dt);
			};
	}
	if (XBendingConstraints)
	{
		ConstraintInits[ConstraintInitIndex++] =
			[this](const TPBDParticles<float, 3>& /*Particles*/)
			{
				XBendingConstraints->Init();
			};
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				XBendingConstraints->Apply(Particles, Dt);
			};
	}
	if (BendingConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				BendingConstraints->Apply(Particles, Dt);
			};
	}
	if (BendingElementConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				BendingElementConstraints->Apply(Particles, Dt);
			};
	}
	if (XAreaConstraints)
	{
		ConstraintInits[ConstraintInitIndex++] =
			[this](const TPBDParticles<float, 3>& /*Particles*/)
			{
				XAreaConstraints->Init();
			};
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				XAreaConstraints->Apply(Particles, Dt);
			};
	}
	if (AreaConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				AreaConstraints->Apply(Particles, Dt);
			};
	}
	if (ThinShellVolumeConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				ThinShellVolumeConstraints->Apply(Particles, Dt);
			};
	}
	if (VolumeConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				VolumeConstraints->Apply(Particles, Dt);
			};
	}
	if (XLongRangeConstraints)
	{
		ConstraintInits[ConstraintInitIndex++] =
			[this](const TPBDParticles<float, 3>& /*Particles*/)
			{
				XLongRangeConstraints->Init();
			};
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				XLongRangeConstraints->Apply(Particles, Dt);
			};
	}
	if (LongRangeConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				LongRangeConstraints->Apply(Particles, Dt);
			};
	}
	if (MaximumDistanceConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				MaximumDistanceConstraints->SetSphereRadiiMultiplier(FMath::Max(0.f, MaxDistancesMultiplier));
				MaximumDistanceConstraints->Apply(Particles, Dt);
			};
	}
	if (BackstopConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				BackstopConstraints->Apply(Particles, Dt);
			};
	}
	if (AnimDriveConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				AnimDriveConstraints->SetSpringStiffness(FMath::Clamp(AnimDriveSpringStiffness, 0.f, 1.f));
				AnimDriveConstraints->Apply(Particles, Dt);
			};
	}
	if (ShapeConstraints)
	{
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				ShapeConstraints->Apply(Particles, Dt);
			};
	}
	if (SelfCollisionConstraints)
	{
		ConstraintInits[ConstraintInitIndex++] =
			[this](const TPBDParticles<float, 3>& Particles)
			{
				SelfCollisionConstraints->Init(Particles);
			};
		ConstraintRules[ConstraintRuleIndex++] =
			[this](TPBDParticles<float, 3>& Particles, const float Dt)
			{
				SelfCollisionConstraints->Apply(Particles, Dt);
			};
	}
	check(ConstraintInitIndex == NumConstraintInits);
	check(ConstraintRuleIndex == NumConstraintRules);
}

void FClothConstraints::SetEdgeConstraints(const TArray<TVec3<int32>>& SurfaceElements, float EdgeStiffness, bool bUseXPBDConstraints)
{
	check(Evolution);
	check(EdgeStiffness > 0.f && EdgeStiffness <= 1.f);

	if (bUseXPBDConstraints)
	{
		XEdgeConstraints = MakeShared<TXPBDSpringConstraints<float, 3>>(Evolution->Particles(), SurfaceElements, EdgeStiffness, /*bStripKinematicConstraints =*/ true);
		++NumConstraintInits;
	}
	else
	{
		EdgeConstraints = MakeShared<FPBDSpringConstraints>(Evolution->Particles(), SurfaceElements, EdgeStiffness, /*bStripKinematicConstraints =*/ true);
	}
	++NumConstraintRules;
}

void FClothConstraints::SetBendingConstraints(TArray<TVec2<int32>>&& Edges, float BendingStiffness, bool bUseXPBDConstraints)
{
	check(Evolution);

	if (bUseXPBDConstraints)
	{
		XBendingConstraints = MakeShared<TXPBDSpringConstraints<float, 3>>(Evolution->Particles(), MoveTemp(Edges), BendingStiffness, /*bStripKinematicConstraints =*/ true);
		++NumConstraintInits;
	}
	else
	{
		BendingConstraints = MakeShared<FPBDSpringConstraints>(Evolution->Particles(), MoveTemp(Edges), BendingStiffness, /*bStripKinematicConstraints =*/ true);
	}
	++NumConstraintRules;
}

void FClothConstraints::SetBendingConstraints(TArray<TVec4<int32>>&& BendingElements, float BendingStiffness)
{
	check(Evolution);
	check(BendingStiffness > 0.f && BendingStiffness <= 1.f);

	BendingElementConstraints = MakeShared<TPBDBendingConstraints<float>>(Evolution->Particles(), MoveTemp(BendingElements), BendingStiffness);  // TODO: Strip kinematic constraints
	++NumConstraintRules;
}

void FClothConstraints::SetAreaConstraints(TArray<TVec3<int32>>&& SurfaceElements, float AreaStiffness, bool bUseXPBDConstraints)
{
	check(Evolution);
	check(AreaStiffness > 0.f && AreaStiffness <= 1.f);

	if (bUseXPBDConstraints)
	{
		XAreaConstraints = MakeShared<TXPBDAxialSpringConstraints<float, 3>>(Evolution->Particles(), MoveTemp(SurfaceElements), AreaStiffness);
		++NumConstraintInits;
	}
	else
	{
		AreaConstraints = MakeShared<FPBDAxialSpringConstraints>(Evolution->Particles(), MoveTemp(SurfaceElements), AreaStiffness);
	}
	++NumConstraintRules;
}

void FClothConstraints::SetVolumeConstraints(TArray<TVec2<int32>>&& DoubleBendingEdges, float VolumeStiffness)
{
	check(Evolution);
	check(VolumeStiffness > 0.f && VolumeStiffness <= 1.f);

	ThinShellVolumeConstraints = MakeShared<FPBDSpringConstraints>(Evolution->Particles(), MoveTemp(DoubleBendingEdges), VolumeStiffness);
	++NumConstraintRules;
}

void FClothConstraints::SetVolumeConstraints(TArray<TVec3<int32>>&& SurfaceElements, float VolumeStiffness)
{
	check(Evolution);
	check(VolumeStiffness > 0.f && VolumeStiffness <= 1.f);

	VolumeConstraints = MakeShared<TPBDVolumeConstraint<float>>(Evolution->Particles(), MoveTemp(SurfaceElements), VolumeStiffness);
	++NumConstraintRules;
}

void FClothConstraints::SetLongRangeConstraints(const TMap<int32, TSet<int32>>& PointToNeighborsMap, float StrainLimitingStiffness, float LimitScale, ETetherMode TetherMode, bool bUseXPBDConstraints)
{
	check(Evolution);
	check(StrainLimitingStiffness > 0.f && StrainLimitingStiffness <= 1.f);

	static const int32 MaxNumTetherIslands = 4;  // The max number of connected neighbors per particle.

	if (bUseXPBDConstraints)
	{
		XLongRangeConstraints = MakeShared<TXPBDLongRangeConstraints<float, 3>>(
			Evolution->Particles(),
			PointToNeighborsMap,
			MaxNumTetherIslands,
			StrainLimitingStiffness);  // TODO: Add LimitScale to the XPBD constraint
		++NumConstraintInits;
	}
	else
	{
		LongRangeConstraints = MakeShared<TPBDLongRangeConstraints<float, 3>>(
			Evolution->Particles(),
			PointToNeighborsMap,
			MaxNumTetherIslands,
			StrainLimitingStiffness,
			LimitScale,
			TetherMode);
	}
	++NumConstraintRules;
}

void FClothConstraints::SetMaximumDistanceConstraints(const TConstArrayView<float>& MaxDistances)
{
	MaximumDistanceConstraints = MakeShared<TPBDSphericalConstraint<float, 3>>(
		ParticleOffset,
		NumParticles,
		*AnimationPositions,
		MaxDistances);
	++NumConstraintRules;
}

void FClothConstraints::SetBackstopConstraints(const TConstArrayView<float>& BackstopDistances, const TConstArrayView<float>& BackstopRadiuses, bool bUseLegacyBackstop)
{
	BackstopConstraints = MakeShared<TPBDSphericalBackstopConstraint<float, 3>>(
		ParticleOffset,
		NumParticles,
		*AnimationPositions,
		*AnimationNormals,
		BackstopRadiuses,
		BackstopDistances,
		bUseLegacyBackstop);
	++NumConstraintRules;
}

void FClothConstraints::SetAnimDriveConstraints(const TConstArrayView<float>& AnimDriveMultipliers)
{
	AnimDriveConstraints = MakeShared<TPBDAnimDriveConstraint<float, 3>>(
		ParticleOffset,
		NumParticles,
		*AnimationPositions,
		AnimDriveMultipliers);
	++NumConstraintRules;
}

void FClothConstraints::SetShapeTargetConstraints(float ShapeTargetStiffness)
{
	// TODO: Review this constraint. Currently does nothing more than the anim drive with less controls
	check(ShapeTargetStiffness > 0.f && ShapeTargetStiffness <= 1.f);

	ShapeConstraints = MakeShared<TPBDShapeConstraints<float, 3>>(
		ParticleOffset,
		NumParticles,
		*AnimationPositions,
		*AnimationPositions,
		ShapeTargetStiffness);
	++NumConstraintRules;
}

void FClothConstraints::SetSelfCollisionConstraints(const TArray<TVec3<int32>>& SurfaceElements, TSet<TVec2<int32>>&& DisabledCollisionElements, float SelfCollisionThickness)
{
	SelfCollisionConstraints = MakeShared<TPBDCollisionSpringConstraints<float, 3>>(
		ParticleOffset,
		NumParticles,
		SurfaceElements,
		MoveTemp(DisabledCollisionElements),
		SelfCollisionThickness,
		/*Stiffness =*/ 1.f);
	++NumConstraintInits;  // Self collision has an init
	++NumConstraintRules;  // and a rule
}

