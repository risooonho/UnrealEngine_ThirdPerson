// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "ItemBase.h"
#include "CharacterModel.h"
#include "CharacterPickupComponent.h"
#include "CharacterInventoryComponent.h"
#include "CharacterAnimInstanceBase.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"

using namespace Wevet;

DEFINE_LOG_CATEGORY(LogWevetClient);

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	MovementSpeed(300.f),
	TakeDamageInterval(0.f),
	ComboTakeInterval(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	HeadBoneName = FName(TEXT("head"));
	HeadSocketName = FName(TEXT("Head_Socket"));
	PelvisSocketName = FName(TEXT("Pelvis_Socket"));
	ChestSocketName = FName(TEXT("Chest_Socket"));

	bCrouch  = false;
	bSprint  = false;
	bWasDied = false;
	bHanging = false;
	bClimbingLedge = false;
	bClimbJumping = false;

	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
	PawnNoiseEmitterComponent->bAutoActivate = 1;
	PawnNoiseEmitterComponent->bAutoRegister = 1;

	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->SetupAttachment(GetMesh());

	PickupComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterPickupComponent>(this, TEXT("PickupComponent"));
	PickupComponent->bAutoActivate = 1;
	PickupComponent->bAutoRegister = 1;

	InventoryComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterInventoryComponent>(this, TEXT("InventoryComponent"));
	InventoryComponent->bAutoActivate = 1;
	InventoryComponent->bAutoRegister = 1;

	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = 1;
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::BeginDestroy()
{
	if (CharacterModel && CharacterModel->IsValidLowLevel())
	{
		CharacterModel->ConditionalBeginDestroy();
	}
	Super::BeginDestroy();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	if (ComponentExtension::HasValid(PawnNoiseEmitterComponent))
	{
		//
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(TakeDamageInterval))
	{
		TakeDamageInterval -= DeltaTime;
	}
	if (!FMath::IsNearlyZero(ComboTakeInterval))
	{
		ComboTakeInterval -= DeltaTime;
	}
}

float ACharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ACharacterBase::OnReleaseItemExecuter_Implementation() 
{
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

void ACharacterBase::ReportNoise_Implementation(USoundBase* Sound, float Volume)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (Sound)
	{
		//MakeNoise(Volume, this, GetActorLocation());
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::FootStep_Implementation(USoundBase* Sound, float Volume)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	USoundBase* const InSound = Sound ? Sound : FootStepSoundAsset;
	if (InSound)
	{
		//MakeNoise(InVolume, this, GetActorLocation());
		const float Speed = GetVelocity().Size();
		const float InVolume = FMath::Clamp<float>((Speed / GetCharacterMovement()->MaxWalkSpeed), MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, InSound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (Sound)
	{
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(Actor, InVolume, Location);
	}
}

void ACharacterBase::CanGrab_Implementation(bool InCanGrab)
{
	bHanging = InCanGrab;
	GetCharacterMovement()->SetMovementMode(bHanging ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_Walking);
	IGrabExecuter::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	//UE_LOG(LogWevetClient, Log, TEXT("Hanging : %s"), bHanging ? TEXT("true") : TEXT("false"));
}

void ACharacterBase::ClimbLedge_Implementation(bool InClimbLedge)
{
}

void ACharacterBase::ReportClimbEnd_Implementation()
{
	bHanging = false;
	bClimbingLedge = false;
	IGrabExecuter::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	IGrabExecuter::Execute_ClimbLedge(GetCharacterAnimInstance(), bClimbingLedge);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ACharacterBase::ClimbMove_Implementation(float Value)
{
	IGrabExecuter::Execute_ClimbMove(GetCharacterAnimInstance(), Value);
}

void ACharacterBase::ClimbJump_Implementation()
{
	IGrabExecuter::Execute_ClimbJump(GetCharacterAnimInstance());
}

void ACharacterBase::ReportClimbJumpEnd_Implementation()
{
	bHanging = true;
	bCanClimbJumpLeft = false;
	bCanClimbJumpRight = false;
	//bClimbJumping = false;
	IGrabExecuter::Execute_ClimbJump(GetCharacterAnimInstance());
}

void ACharacterBase::TurnConerLeftUpdate_Implementation()
{
}

void ACharacterBase::TurnConerRightUpdate_Implementation()
{
}

void ACharacterBase::TurnConerResult_Implementation()
{
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (bWasDied || CharacterModel == nullptr)
	{
		return true;
	}
	return CharacterModel->IsDie();
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied)
{
	if (BoneName == HeadBoneName) 
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Super::GetMesh();
		auto RefSkeleton = SkeletalMeshComponent->SkeletalMesh->Skeleton->GetReferenceSkeleton();
		if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
		{
			CharacterModel->SetHealth(INDEX_NONE);
		}
	} 
	else
	{
		CharacterModel->TakeDamage((int32)Damage);
	}

	bDied = CharacterModel->IsEmptyHealth();
	if (bDied)
	{
		CharacterModel->Die();
		Die_Implementation();
	}
	else
	{
		TakeDamageActionMontage();
	}
}

void ACharacterBase::InfrictionDamage_Implementation(AActor* InfrictionActor, const bool bInfrictionDie)
{
	//	
}

void ACharacterBase::Die_Implementation()
{
	if (bWasDied)
	{
		return;
	}

	bWasDied = true;
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}

	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetAllBodiesSimulatePhysics(true);
	SkelMesh->SetSimulatePhysics(true);
	SkelMesh->WakeAllRigidBodies();
	SkelMesh->bBlendPhysics = true;

	Super::GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UWorld* const World = GetWorld();
	check(World);
	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (Controller)
	{
		Controller->UnPossess();
	}

	if (!InventoryComponent->HasInventoryWeaponItems())
	{
		for (AWeaponBase*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
		{
			if (!Weapon)
			{
				continue;
			}
			ReleaseWeaponToWorld(Transform, Weapon);
		}
		InventoryComponent->ClearWeaponInventory();
	}
}

void ACharacterBase::Equipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->SetEquip(true);
	}
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->SetEquip(false);
	}
}

UCharacterModel* ACharacterBase::GetPropertyModel_Implementation() const
{
	return CharacterModel;
}

FVector ACharacterBase::BulletTraceRelativeLocation() const
{
	return FVector::ZeroVector;
}

FVector ACharacterBase::BulletTraceForwardLocation() const
{
	return FVector::ZeroVector;
}

void ACharacterBase::Jump()
{
	if (bCrouch)
	{
		OnCrouch();
	}
	Super::Jump();
}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::OnSprint()
{
	bSprint = !bSprint;

	// now crouching slow speed
	if (bCrouch)
	{
		bSprint = false;
	}
	MovementSpeed = bSprint ? DefaultMaxSpeed : DefaultMaxSpeed * HALF_WEIGHT;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	bCrouch = !bCrouch;
	Super::bIsCrouched = bCrouch ? 1 : 0;
	if (bCrouch)
	{
		if (Super::CanCrouch())
		{
			Super::Crouch();
		}
	}
	else
	{
		Super::UnCrouch();
	}
}

float ACharacterBase::GetHealthToWidget() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->GetHealthToWidget();
	}
	return ZERO_VALUE;
}

bool ACharacterBase::IsHealthHalf() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->IsHealthHalf();
	}
	return false;
}

bool ACharacterBase::IsHealthQuarter() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->IsHealthQuarter();
	}
	return false;
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform& Transform, AWeaponBase* &Weapon)
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	const FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
	TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
	Weapon->Release(nullptr);
	Weapon = nullptr;

	AWeaponBase* const SpawningObject = World->SpawnActorDeferred<AWeaponBase>(
		WeaponClass,
		Transform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
	SpawningObject->FinishSpawning(Transform);
}

AWeaponBase* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType)
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return nullptr;
	}
	for (AWeaponBase*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && Weapon->HasMatchTypes(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}

AWeaponBase* ACharacterBase::GetSelectedWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get();
	}
	return nullptr;
}

UCharacterAnimInstanceBase* ACharacterBase::GetCharacterAnimInstance() const
{
	return Cast<UCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}

bool ACharacterBase::HasCrouch() const
{
	return bCrouch;
}

bool ACharacterBase::HasSprint() const
{
	return bSprint;
}

bool ACharacterBase::HasHanging() const
{
	return bHanging;
}

bool ACharacterBase::HasClimbingLedge() const
{
	return bClimbingLedge;
}

bool ACharacterBase::HasClimbingMoveLeft() const
{
	return bCanClimbMoveLeft;
}

bool ACharacterBase::HasClimbingMoveRight() const
{
	return bCanClimbMoveRight;
}

bool ACharacterBase::HasEquipWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->bEquip;
	}
	return false;
}

AWeaponBase* ACharacterBase::GetUnEquipWeapon()
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return nullptr;
	}
	for (AWeaponBase*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && !Weapon->bEquip)
		{
			return Weapon;
		}
	}
	return nullptr;
}

void ACharacterBase::OutUnEquipWeaponList(TArray<AWeaponBase*>& OutWeaponList)
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}
	for (AWeaponBase* &Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && !Weapon->bEquip)
		{
			OutWeaponList.Emplace(Weapon);
		}
	}
}

const bool ACharacterBase::SameWeapon(AWeaponBase* const Weapon)
{
	const EWeaponItemType ItemType = Weapon->WeaponItemInfo.WeaponItemType;
	if (AWeaponBase* const InWeapon = FindByWeapon(ItemType))
	{
		return InWeapon->HasMatchTypes(ItemType);
	}
	return false;
}

void ACharacterBase::PickupObjects()
{
}

void ACharacterBase::ReleaseObjects()
{
}

void ACharacterBase::EquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}
	PlayAnimMontage(EquipMontage, MONTAGE_DELAY);
}

void ACharacterBase::UnEquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}
	PlayAnimMontage(UnEquipMontage, MONTAGE_DELAY);
}

void ACharacterBase::FireActionMontage()
{
	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		switch (WeaponType)
		{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
			{
				PlayAnimMontage(FireMontage);
			}
			break;
			case EWeaponItemType::Bomb:
			{
				//
			}
			break;
			case EWeaponItemType::Pistol:
			{
				//
			}
			break;
		}
	}
}

void ACharacterBase::ReloadActionMontage()
{
	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		switch (WeaponType)
		{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
			{
				PlayAnimMontage(ReloadMontage);
			}
			break;
			case EWeaponItemType::Bomb:
			{
				//
			}
			break;
			case EWeaponItemType::Pistol:
			{
				//
			}
			break;
		}
	}
}

void ACharacterBase::TakeDamageActionMontage()
{
	if (FMath::IsNearlyZero(TakeDamageInterval))
	{
		if (CurrentWeapon.IsValid())
		{
			const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
			switch (WeaponType)
			{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
				if (RifleHitDamageMontage)
				{
					TakeDamageInterval = PlayAnimMontage(RifleHitDamageMontage);
				}
				break;
			case EWeaponItemType::Bomb:
				//
				break;
			case EWeaponItemType::Pistol:
				//
				break;
			};
		}
		else
		{
			if (DefaultHitDamageMontage)
			{
				TakeDamageInterval = PlayAnimMontage(DefaultHitDamageMontage);
			}
		}
	}
}


FVector ACharacterBase::GetHeadSocketLocation() const
{
	FVector Position = GetActorLocation();
	if (GetMesh())
	{
		// FindBone
		if (GetMesh()->GetBoneIndex(HeadSocketName) != INDEX_NONE)
		{
			FVector BoneLocation = GetMesh()->GetBoneLocation(HeadSocketName);
			Position.Z = BoneLocation.Z;
		}
		else
		{
			// Not Found Try FindSocket
			FVector SocketLocation = GetMesh()->GetSocketLocation(HeadSocketName);
			if (SocketLocation != FVector::ZeroVector)
			{
				Position.Z = SocketLocation.Z;
			}
		}
	}
	return Position;
}

FVector ACharacterBase::GetChestSocketLocation() const
{
	FVector Position = GetActorLocation();
	if (GetMesh())
	{
		// FindBone
		if (GetMesh()->GetBoneIndex(ChestSocketName) != INDEX_NONE)
		{
			FVector BoneLocation = GetMesh()->GetBoneLocation(ChestSocketName);
			Position.Z = BoneLocation.Z;
		}
		else
		{
			// Not Found Try FindSocket
			FVector SocketLocation = GetMesh()->GetSocketLocation(ChestSocketName);
			if (SocketLocation != FVector::ZeroVector)
			{
				Position.Z = SocketLocation.Z;
			}
		}
	}
	return Position;
}