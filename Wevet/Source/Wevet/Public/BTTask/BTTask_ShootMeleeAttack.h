// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ShootMeleeAttack.generated.h"

/**
 *
 */
UCLASS()
class WEVET_API UBTTask_ShootMeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ShootMeleeAttack();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShootMeleeAttack|Variable")
	float MinBulletInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShootMeleeAttack|Variable")
	float MaxBulletInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShootMeleeAttack|Variable")
	float MeleeAttackTimeOut;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
