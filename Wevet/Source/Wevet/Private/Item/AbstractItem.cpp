// Copyright © 2018 wevet works All Rights Reserved.


#include "Item/AbstractItem.h"
#include "Character/CharacterBase.h"

AAbstractItem::AAbstractItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbstractItem::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
}

void AAbstractItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAbstractItem::Take_Implementation(ACharacterBase* NewCharacter)
{

}

void AAbstractItem::Release_Implementation(ACharacterBase* NewCharacter)
{

}

EItemType AAbstractItem::GetItemType_Implementation() const
{
	return ItemType;
}

void AAbstractItem::HitReceive(
	UPrimitiveComponent* HitComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	FVector NormalImpulse, 
	const FHitResult& Hit)
{
}

void AAbstractItem::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
}

void AAbstractItem::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
}

