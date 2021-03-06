// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/AbstractWeapon.h"
#include "Weapon/BulletBase.h"
#include "Character/CharacterModel.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Interface/AIPawnOwner.h"

AAbstractWeapon::AAbstractWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MuzzleSocketName(FName(TEXT("MuzzleFlash")))
{
	PrimaryActorTick.bCanEverTick = true;
	bEquip  = false;
	bReload = false;
	bDebugTrace = false;

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	RootComponent = SkeletalMeshComponent;
	SkeletalMeshComponent->bRenderCustomDepth = false;
	SkeletalMeshComponent->CustomDepthStencilValue = 1;
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::ComponentTransformIsKinematic;
	SkeletalMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SkeletalMeshComponent->SetEnableGravity(false);

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Engine/VREditor/Sounds/UI/Object_PickUp"));
		PickupSound = FindAsset.Object;
	}
}

void AAbstractWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	AddDelegate();
}

void AAbstractWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetCharacterOwner();
	PrepareDestroy();
	Super::EndPlay(EndPlayReason);
}

void AAbstractWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region Initialize
void AAbstractWeapon::Initialize(ACharacterBase* const NewCharacterOwner)
{
	CharacterPtr = MakeWeakObjectPtr<ACharacterBase>(NewCharacterOwner);
	if (GetPawnOwner())
	{
		IgnoreActors.Add(GetPawnOwner());
		IgnoreActors.Add(this);
		Super::SetOwner(GetPawnOwner());
	}
}

void AAbstractWeapon::ResetCharacterOwner()
{
	CharacterPtr.Reset();
	IgnoreActors.Reset(0);
	Super::SetOwner(nullptr);
}
#pragma endregion

ACharacterBase* AAbstractWeapon::GetPointer() const
{
	if (CharacterPtr.IsValid())
	{
		return CharacterPtr.Get();
	}
	return nullptr;
}

APawn* AAbstractWeapon::GetPawnOwner() const
{
	if (CharacterPtr.IsValid())
	{
		return CharacterPtr.Get();
	}
	return nullptr;
}

#pragma region Interface
void AAbstractWeapon::DoFirePressed_Implementation() 
{
	// @TODO
}

void AAbstractWeapon::DoFireRelease_Implementation()
{
	// @TODO
}

// Reload
void AAbstractWeapon::DoReload_Implementation()
{
	if (WeaponItemInfo.MaxAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Empty Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	if (WeaponItemInfo.CurrentAmmo >= WeaponItemInfo.ClipType)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Full Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	SetReload(true);
	ReloadDuration = DEFAULT_VALUE;
	CharacterPtr.Get()->ReloadActionMontage(ReloadDuration);
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
	}

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		SetReload(false);
	});
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, TimerCallback, ReloadDuration, false);
}

// 補給
void AAbstractWeapon::DoReplenishment_Implementation(const FWeaponItemInfo& RefWeaponItemInfo)
{
	WeaponItemInfo.MaxAmmo += RefWeaponItemInfo.MaxAmmo;
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);

	const bool bWasNeededAmmo = (WeaponItemInfo.MaxAmmo <= NeededAmmo);
	WeaponItemInfo.MaxAmmo = bWasNeededAmmo ? 0 : (WeaponItemInfo.MaxAmmo - NeededAmmo);
	WeaponItemInfo.CurrentAmmo = bWasNeededAmmo ? (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo) : WeaponItemInfo.ClipType;
}

// 攻撃可能か?
bool AAbstractWeapon::CanMeleeStrike_Implementation() const
{
	// was nullptr
	if (!CharacterPtr.IsValid())
	{
		return false;
	}

	// was death
	if (IDamageInstigator::Execute_IsDeath(CharacterPtr.Get()))
	{
		return false;
	}
	bool bWasEmpty = (WeaponItemInfo.MaxAmmo <= 0);
	return (bWasEmpty == false);
}

// TakeWeapon
void AAbstractWeapon::Take_Implementation(ACharacterBase* NewCharacter)
{
	ResetCharacterOwner();
	Initialize(NewCharacter);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	UpdateCustomDepth(false);
	RemoveDelegate();
	if (CollisionComponent)
	{
		CollisionComponent->ConditionalBeginDestroy();
		CollisionComponent = nullptr;
	}

	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Release Weapon
void AAbstractWeapon::Release_Implementation(ACharacterBase* NewCharacter)
{
	ResetCharacterOwner();
	SetEquip(false);

	if (IsValidLowLevel())
	{
		PrepareDestroy();
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}	
}

// アイテム種別
EItemType AAbstractWeapon::GetItemType_Implementation() const
{
	return EItemType::Weapon;
}
#pragma endregion

#pragma region HitEvent
void AAbstractWeapon::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor))
	{
		if (!IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
		{
			return;
		}
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		UpdateCustomDepth(true);
		Character->GetPickupComponent()->SetPickupActor(this);
	}
}

void AAbstractWeapon::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor))
	{
		if (!IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
		{
			return;
		}
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		UpdateCustomDepth(false);
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}
#pragma endregion

void AAbstractWeapon::CheckIfValid(AActor* OtherActor, bool& bCanDamageResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	// Owner nullptr || Same Owner Hit Actor
	if (GetPawnOwner() == nullptr || (GetPawnOwner() && GetPawnOwner() == OtherActor))
	{
		return;
	}

	IDamageInstigator* DamageInstigator = Cast<IDamageInstigator>(OtherActor);
	if (DamageInstigator == nullptr)
	{
		return;
	}

	// Already Death
	if (IDamageInstigator::Execute_IsDeath(DamageInstigator->_getUObject()))
	{
		//
	}

	// if AIOwner Overlap hit AIOwner
	if (IAIPawnOwner* AI = Cast<IAIPawnOwner>(GetPawnOwner()))
	{
		if (OtherActor->IsA(GetPawnOwner()->GetClass()))
		{
			return;
		}
	}
	bCanDamageResult = true;
}

void AAbstractWeapon::TakeDamageOuter(const FHitResult& HitResult)
{
	AActor* OtherActor = HitResult.GetActor();
	if (OtherActor == nullptr || GetPawnOwner() == nullptr)
	{
		return;
	}

	// Same Actor or Hit Actor Owner
	if ((OtherActor && OtherActor == this) || (OtherActor && OtherActor == GetPawnOwner()))
	{
		return;
	}

	if (!OtherActor->IsValidLowLevel() && OtherActor->IsPendingKill())
	{
		return;
	}

	IDamageInstigator* DamageInstigator = Cast<IDamageInstigator>(OtherActor);
	if (DamageInstigator == nullptr)
	{
		return;
	}

	float TotalDamage = ZERO_VALUE;
	UCharacterModel* const DamageModel = IDamageInstigator::Execute_GetPropertyModel(DamageInstigator->_getUObject());
	IDamageInstigator::Execute_MakeDamage(GetPawnOwner(), DamageModel, WeaponItemInfo.Damage, TotalDamage);

	// @NOTE
	// Head Shot Overkill
	if (IDamageInstigator::Execute_CanKillDealDamage(OtherActor, HitResult.BoneName))
	{
		TotalDamage += (float)DamageModel->GetHealth();
		UE_LOG(LogWevetClient, Warning, TEXT("KillDealDamage : %s, CurrentHealth : %d, TotalDamage : %f"), *HitResult.BoneName.ToString(), DamageModel->GetHealth(), TotalDamage);
	}

	// @NOTE
	// Blood Effect
	IDamageInstigator::Execute_HitEffectReceive(OtherActor, HitResult);

	// @TODO
	TSubclassOf<UDamageType> DamageType;
	UGameplayStatics::ApplyPointDamage(
		OtherActor, 
		TotalDamage, 
		HitResult.ImpactPoint,
		HitResult,
		GetPointer()->GetController(), 
		this, 
		DamageType);
}

// AI override Method
void AAbstractWeapon::OnFirePressInternal()
{
	if (!CanMeleeStrike_Implementation())
	{
		return;
	}

	if (bReload)
	{
		return;
	}

	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Reload Ammos"));
		DoReload_Implementation();
		return;
	}

	const FVector StartLocation = GetPointer()->BulletTraceRelativeLocation();
	const FVector ForwardLocation = GetPointer()->BulletTraceForwardLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName(TEXT("DamageInstigator"));
	CollisionQueryParams.OwnerTag = FName(TEXT("Character"));
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActors(IgnoreActors);

	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

	const FVector MuzzleLocation = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	ISoundInstigator::Execute_ReportNoiseOther(GetPointer(), this, FireSound, DEFAULT_VOLUME, MuzzleLocation);
	GetPointer()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	ISoundInstigator::Execute_ReportNoiseOther(GetPointer(), this, ImpactSound, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	if (bDebugTrace)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartPoint, EndPoint, FLinearColor::Blue, 2.0f, 2.0f);
	}

	SpawnBullet(Transform);
	OnHitTriggerDelegate(HitData.GetActor(), HitData);
}

void AAbstractWeapon::ReloadBulletParams()
{
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);
	const bool bWasNeededAmmo = (WeaponItemInfo.MaxAmmo <= NeededAmmo);
	WeaponItemInfo.MaxAmmo = bWasNeededAmmo ? 0 : (WeaponItemInfo.MaxAmmo - NeededAmmo);
	WeaponItemInfo.CurrentAmmo = bWasNeededAmmo ? (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo) : WeaponItemInfo.ClipType;
}

void AAbstractWeapon::PrepareDestroy()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
		TimerManager.ClearAllTimersForObject(this);
	}
}

void AAbstractWeapon::SpawnBullet(const FTransform InTransform)
{
	if (BulletsTemplate)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetPawnOwner();
		ABulletBase* const Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletsTemplate, InTransform, SpawnParams);

		if (Bullet)
		{
#if WITH_EDITOR
			Bullet->SetFolderPath("/BulletsRoot");
#endif
			Bullet->SetOwners(IgnoreActors);
			Bullet->HitTriggerDelegate.AddDynamic(this, &AAbstractWeapon::OnHitTriggerDelegate);
		}
	}
}

void AAbstractWeapon::OnHitTriggerDelegate(AActor* const OtherActor, const FHitResult HitResult)
{
	bool bCanDamageResult = false;
	CheckIfValid(HitResult.GetActor(), bCanDamageResult);
	if (bCanDamageResult)
	{
		TakeDamageOuter(HitResult);
	}
	PlayEffect(HitResult, SkeletalMeshComponent);
}

void AAbstractWeapon::PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent)
{
	if (HitResult.GetActor())
	{
		FTransform EmitterTransform;
		EmitterTransform.SetIdentity();
		EmitterTransform.SetLocation(HitResult.ImpactPoint);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitterTemplate, EmitterTransform, true);
	}

	UGameplayStatics::SpawnEmitterAttached(
		FlashEmitterTemplate, 
		InComponent, 
		MuzzleSocketName, 
		GetMuzzleTransform().GetLocation(), 
		FRotator(GetMuzzleTransform().GetRotation()), 
		EAttachLocation::KeepWorldPosition, 
		true);
}

void AAbstractWeapon::CopyWeaponItemInfo(const FWeaponItemInfo* RefWeaponItemInfo)
{
	WeaponItemInfo.UnEquipSocketName = RefWeaponItemInfo->UnEquipSocketName;
	WeaponItemInfo.EquipSocketName = RefWeaponItemInfo->EquipSocketName;
	WeaponItemInfo.WeaponItemType = RefWeaponItemInfo->WeaponItemType;
	WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo->CurrentAmmo;
	WeaponItemInfo.ClipType = RefWeaponItemInfo->ClipType;
	WeaponItemInfo.MaxAmmo = RefWeaponItemInfo->MaxAmmo;
	WeaponItemInfo.Damage = RefWeaponItemInfo->Damage;
	WeaponItemInfo.Texture = RefWeaponItemInfo->Texture;
}

void AAbstractWeapon::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetOwnerNoSee(NewOwnerNoSee);
	}
}

void AAbstractWeapon::UpdateCustomDepth(const bool bEnable)
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetRenderCustomDepth(bEnable);
		SkeletalMeshComponent->CustomDepthStencilValue = bEnable ? 1 : 0;
	}
}

#pragma region Delegate
void AAbstractWeapon::AddDelegate()
{
	if (!CollisionComponent)
	{
		return;
	}
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractWeapon::BeginOverlapRecieve);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAbstractWeapon::EndOverlapRecieve);
}

void AAbstractWeapon::RemoveDelegate()
{
	if (!CollisionComponent)
	{
		return;
	}
	if (CollisionComponent->OnComponentBeginOverlap.IsBound())
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractWeapon::BeginOverlapRecieve);
	}
	if (CollisionComponent->OnComponentEndOverlap.IsBound())
	{
		CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAbstractWeapon::EndOverlapRecieve);
	}

}
#pragma endregion

