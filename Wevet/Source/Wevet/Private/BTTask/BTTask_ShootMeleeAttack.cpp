// Copyright © 2018 wevet works All Rights Reserved.


#include "BTTask_ShootMeleeAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "Engine.h"

UBTTask_ShootMeleeAttack::UBTTask_ShootMeleeAttack()
	: Super()
{
	MinBulletInterval = 0.01f;
	MaxBulletInterval = 0.1f;
	MeleeAttackTimeOut = 8.f;
	NodeName = TEXT("BTTask_ShootMeleeAttack");
	BlackboardKey.SelectedKeyName = FName(TEXT("Target"));
}

EBTNodeResult::Type UBTTask_ShootMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		AAICharacterBase* AICharacter = Cast<AAICharacterBase>(AIController->GetPawn());
		if (!AICharacter)
		{
			return EBTNodeResult::Failed;
		}

		if (!AICharacter->GetTarget_Implementation())
		{
			UE_LOG(LogWevetClient, Log, TEXT("LostTarget.. %s"), *FString(__FUNCTION__));
			return EBTNodeResult::Failed;
		}

		if (AICharacter->WasMeleeAttacked())
		{
			UE_LOG(LogWevetClient, Log, TEXT("MeleeAttackPlaying.. %s"), *FString(__FUNCTION__));
			return EBTNodeResult::InProgress;
		}

		AICharacter->SetBulletInterval(FMath::Abs(FMath::RandRange(MinBulletInterval, MaxBulletInterval)));
		AICharacter->SetMeleeAttackTimeOut(MeleeAttackTimeOut);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

