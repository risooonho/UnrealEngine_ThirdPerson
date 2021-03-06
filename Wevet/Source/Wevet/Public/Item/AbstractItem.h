// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WevetTypes.h"
#include "Structs/WeaponItemInfo.h"
#include "Interface/InteractionInstigator.h"
#include "AbstractItem.generated.h"

class ACharacterBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractItem : public AActor, public IInteractionInstigator
{
	GENERATED_BODY()
	
public:	
	AAbstractItem(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
#pragma region InteractionInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractItem|InteractionInstigator")
	void Take(ACharacterBase* NewCharacter);
	virtual void Take_Implementation(ACharacterBase* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractItem|InteractionInstigator")
	void Release(ACharacterBase* NewCharacter);
	virtual void Release_Implementation(ACharacterBase* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractItem|InteractionInstigator")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const override;
#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractItem|Variable")
	EItemType ItemType;

	UFUNCTION(BlueprintCallable, Category = "AbstractItem|Function")
	virtual void HitReceive(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "AbstractItem|Function")
	virtual void BeginOverlapRecieve(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "AbstractItem|Function")
	virtual	void EndOverlapRecieve(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractItem|Weapon")
	FWeaponItemInfo WeaponItemInfo;
};
