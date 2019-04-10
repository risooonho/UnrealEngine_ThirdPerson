// Copyright © 2018 wevet works All Rights Reserved.

#include "BTTask_MoveNearCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "AICharacterBase.h"
#include "Engine.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTask_MoveNearCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bool bSuccess = false;
	FVector SearchOrigin = FVector::ZeroVector;

	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		if (AIController->GetTargetCharacter())
		{
			// @NOTE
			// Keep SearchOrigin PlayerLocation
			const float SearchRadius = AIController->GetSearchRadius();
			SearchOrigin = AIController->GetTargetCharacter()->GetActorLocation();
			FNavLocation ResultLocation;

			UNavigationSystemV1* NavSystem  = UNavigationSystemV1::GetNavigationSystem(AIController);
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
			{
				AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
				bSuccess = true;
				auto AIName = AIController->GetAICharacter()->GetName();
				UE_LOG(LogWevetClient, Log, TEXT("Success NearPlayer : %s \n Position : %s"), *AIName, *SearchOrigin.ToString());
			}
		}
		else
		{
			SearchOrigin = AIController->GetAICharacter()->GetActorLocation();
			AIController->SetBlackboardPatrolLocation(SearchOrigin);
		}
	}
	return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

