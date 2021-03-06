// Copyright © 2018 wevet works All Rights Reserved.


#include "AI/AISpawner.h"
#include "Engine/World.h"
#include "Wevet.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"

AAISpawner::AAISpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SpawnInterval(1.f)
{
	SpawnCount = 10;
	bSpawnFinished = false;
	PrimaryActorTick.bCanEverTick = true;
	bInifinitySpawn = false;
	SpawnRadius = 200.f;
}

void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnPoints.Reset(0);
	RestartSpawn();
}

void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpawnInterval >= SpawnTimer)
	{
		SpawnTimer += DeltaTime;
	}
	else
	{
		if (!bSpawnFinished)
		{
			SpawnTimer = ZERO_VALUE;
			DoSpawn();
		}
	}
}

void AAISpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SpawnPoints.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void AAISpawner::RestartSpawn()
{
	SpawnTimer = ZERO_VALUE;
	bSpawnFinished = false;
	UWevetBlueprintFunctionLibrary::CircleSpawnPoints(SpawnCount, SpawnRadius, GetActorLocation(), SpawnPoints);
	Super::SetActorTickEnabled(true);
}

void AAISpawner::DoSpawn()
{
	bSpawnFinished = (CurrentSpawnCount >= SpawnCount && !bInifinitySpawn);
	if (bSpawnFinished)
	{
#if WITH_EDITOR
		UE_LOG(LogWevetClient, Warning, TEXT("LimitOver \n Init : %d \n Max : %d"), SpawnCount, CurrentSpawnCount);
#endif
		Super::SetActorTickEnabled(false);
		return;
	}

	if (CharacterTemplate == nullptr)
	{
#if WITH_EDITOR
		UE_LOG(LogWevetClient, Error, TEXT("Not Setting BPTemplate : %s"), *FString(__FUNCTION__));
#endif
		Super::SetActorTickEnabled(false);
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform;
	Transform.SetLocation(SpawnPoints[CurrentSpawnCount]);
	AAICharacterBase* SpawningObject = World->SpawnActor<AAICharacterBase>(CharacterTemplate, Transform, SpawnParams);

	if (SpawningObject)
	{
		++CurrentSpawnCount;
		SpawningObject->SetInstigator(GetInstigator());
	}

#if WITH_EDITOR
	if (SpawningObject)
	{
		SpawningObject->SetFolderPath("Enemy");
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("Spawn Error : %s"), *FString(__FUNCTION__));
	}
#endif
}
