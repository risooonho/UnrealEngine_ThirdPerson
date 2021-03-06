// Copyright © 2018 wevet works All Rights Reserved.


#include "Environment/WaterActor.h"
#include "Player/MockCharacter.h"

AWaterActor::AWaterActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWaterActor::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
}

void AWaterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWaterActor::SetTarget(class AMockCharacter* InTarget)
{
	Target = InTarget;
	Super::SetActorTickEnabled(Target ? true : false);
}
