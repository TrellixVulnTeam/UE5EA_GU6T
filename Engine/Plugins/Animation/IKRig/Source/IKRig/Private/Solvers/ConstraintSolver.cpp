// Copyright Epic Games, Inc. All Rights Reserved.

#include "Solvers/ConstraintSolver.h"
#include "IKRigBoneSetting.h"

void UIKRigConstraintSolver::Init(const FIKRigTransforms& InGlobalTransform)
{
	// TODO get constraints that this solver can deal with
}

void UIKRigConstraintSolver::Solve(
	FIKRigTransforms& InOutGlobalTransform,
	const FIKRigGoalContainer& Goals,
	FControlRigDrawInterface* InOutDrawInterface)
{
	/*
	FIKRigConstraintProfile* Current = ConstraintProfiles.Find(ActiveProfile);

	if (Current)
	{
		for (auto Iter = Current->Constraints.CreateIterator(); Iter; ++Iter)
		{
			UIKRigConstraint* Constraint = Iter.Value();
			if (Constraint)
			{
				// @todo: later add inoutdrawinterface
				Constraint->Apply(InOutGlobalTransform, InOutDrawInterface);
			}
		}
	}*/
}

void UIKRigConstraintSolver::CollectGoalNames(TSet<FName>& OutGoals) const
{

}