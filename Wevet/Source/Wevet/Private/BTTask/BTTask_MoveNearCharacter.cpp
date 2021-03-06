// Copyright © 2018 wevet works All Rights Reserved.

#include "BTTask_MoveNearCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "Engine.h"
#include "NavigationSystem.h"
#include "Interface/AIPawnOwner.h"

EBTNodeResult::Type UBTTask_MoveNearCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FVector SearchOrigin = FVector::ZeroVector;

	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		APawn* AIPawn = AIController->GetPawn();
		IAIPawnOwner* Interface = Cast<IAIPawnOwner>(AIPawn);
		if (Interface)
		{
			const float SearchRadius = IAIPawnOwner::Execute_GetAttackTraceForwardDistance(Interface->_getUObject());
			APawn* const Pawn = Cast<APawn>(IAIPawnOwner::Execute_GetTarget(Interface->_getUObject()));
			SearchOrigin = Pawn->GetActorLocation();
			FNavLocation ResultLocation;
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
			{
				AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
				UE_LOG(
					LogWevetClient, 
					Log, 
					TEXT("Success NearPlayer : %s \n Position : %s"), 
					*AIPawn->GetName(), 
					*SearchOrigin.ToString());
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

