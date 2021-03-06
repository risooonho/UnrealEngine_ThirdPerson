// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterActor.generated.h"

class AMockCharacter;

UCLASS()
class WEVET_API AWaterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaterActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	class AMockCharacter* Target;

	UFUNCTION(BlueprintCallable, Category = "WaterActor|Function")
	void SetTarget(class AMockCharacter* InTarget);
};
