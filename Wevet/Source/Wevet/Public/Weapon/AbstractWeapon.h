// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/WeaponInstigator.h"
#include "Interface/InteractionInstigator.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "WevetTypes.h"
#include "AbstractWeapon.generated.h"

class ACharacterBase;
class ABulletBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AActor, public IWeaponInstigator, public IInteractionInstigator
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFireRelease();
	virtual void DoFireRelease_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReplenishment(const FWeaponItemInfo& RefWeaponItemInfo);
	virtual void DoReplenishment_Implementation(const FWeaponItemInfo& RefWeaponItemInfo) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Take(ACharacterBase* NewCharacter);
	virtual void Take_Implementation(ACharacterBase* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Release(ACharacterBase* NewCharacter);
	virtual void Release_Implementation(ACharacterBase* NewCharacter) override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const override;
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent;

public:
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

public:
	virtual void Initialize(ACharacterBase* const NewCharacterOwner);
	virtual void ResetCharacterOwner();

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	ACharacterBase* GetPointer() const;

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	APawn* GetPawnOwner() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FWeaponItemInfo WeaponItemInfo;

	void ReloadBulletParams();

protected:
	TWeakObjectPtr<ACharacterBase> CharacterPtr;
	TArray<class AActor*> IgnoreActors;
	FTimerHandle ReloadTimerHandle;

	void CheckIfValid(AActor* OtherActor, bool& bCanDamageResult);
	void TakeDamageOuter(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void OnFirePressInternal();

	UFUNCTION()
	virtual void PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent);

	UFUNCTION()
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void PrepareDestroy();

#pragma region Uproperty
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	int32 NeededAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float BulletDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float ReloadDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	bool bDebugTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* FlashEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* ImpactEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class ABulletBase> BulletsTemplate;
#pragma endregion

public:
	FTransform GetMuzzleTransform() const
	{
		return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
	}
	
	void SetEquip(const bool InEquip)
	{
		bEquip = InEquip;
	}

	void SetReload(const bool InReload)
	{
		bReload = InReload;
	}

	FORCEINLINE bool WasEquip() const 
	{
		return bEquip; 
	}

	FORCEINLINE bool WasReload() const
	{
		return bReload; 
	}

	FORCEINLINE bool WasSameWeaponType(EWeaponItemType InWeaponItemType) const
	{
		if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
		{
			return false;
		}
		return WeaponItemInfo.WeaponItemType == InWeaponItemType;
	}

	void CopyWeaponItemInfo(const FWeaponItemInfo* RefWeaponItemInfo);
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

protected:
	void AddDelegate();
	void RemoveDelegate();
	void SpawnBullet(const FTransform InTransform);
	void UpdateCustomDepth(const bool bEnable);

	UFUNCTION()
	void OnHitTriggerDelegate(AActor* const OtherActor, const FHitResult HitResult);
};
