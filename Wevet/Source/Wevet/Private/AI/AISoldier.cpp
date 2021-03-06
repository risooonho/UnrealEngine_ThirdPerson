// Copyright © 2018 wevet works All Rights Reserved.

#include "AI/AISoldier.h"
#include "AI/AIControllerBase.h"
#include "Player/MockCharacter.h"


AAISoldier::AAISoldier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AAISoldier::MainLoop(float DeltaTime)
{
	const float TimeSeconds = GetWorld()->TimeSeconds;
	if (bSeeTarget && (TimeSeconds - LastSeenTime) > SenseTimeOut)
	{
		SetSeeTargetActor(nullptr);
	}
	if (bHearTarget && (TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		SetHearTargetActor(nullptr);
	}

	// @NOTE
	// UBTTask_ShootMeleeAttackから呼ばれる
	if (WasMeleeAttacked())
	{
		MeleeAttackTimeOut -= DeltaTime;
		MeleeAttack();
	}

	if (TargetCharacter)
	{
		UpdateAttackState();
	}
}

void AAISoldier::OnSeePawnRecieve(APawn* OtherPawn)
{
	if (GetTarget_Implementation())
	{
		LastSeenTime += GetWorld()->GetTimeSeconds();
		return;
	}

	AMockCharacter* Character = Cast<AMockCharacter>(OtherPawn);
	if (Character && !IDamageInstigator::Execute_IsDeath(Character))
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		SetSeeTargetActor(Character);
		if (Character)
		{
			ForceSprint();
		}
		else
		{
			UnForceSprint();
		}
	}
}

void AAISoldier::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	if (IsHearTarget_Implementation())
	{
		LastHeardTime += GetWorld()->GetTimeSeconds();
		return;
	}
	LastHeardTime = GetWorld()->GetTimeSeconds();
	SetHearTargetActor(OtherActor);
	if (OtherActor)
	{
		ForceSprint();
	}
	else
	{
		UnForceSprint();
	}
}

void AAISoldier::UpdateAttackState()
{
	if (AIController)
	{
		const EAIActionState ActionState = AIController->GetBlackboardActionState();
		if (ActionState != EAIActionState::Attack)
		{
			if (CanMeleeStrike_Implementation())
			{
				AIController->SetBlackboardActionState(EAIActionState::Attack);
			}
		}
	}
}

void AAISoldier::MeleeAttack()
{
	if (!Super::HasEquipWeapon())
	{
		return;
	}

	bool bWasLostTarget = (TargetCharacter == nullptr) || IDamageInstigator::Execute_IsDeath(TargetCharacter);
	if (bWasLostTarget)
	{
		SetSeeTargetActor(nullptr);
		MeleeAttackTimeOut = ZERO_VALUE;
		return;
	}

	BulletInterval += GetWorld()->GetDeltaSeconds();
	if (BulletInterval >= BulletDelay)
	{
		Super::FirePressed();
		BulletInterval = ZERO_VALUE;
	}
}
