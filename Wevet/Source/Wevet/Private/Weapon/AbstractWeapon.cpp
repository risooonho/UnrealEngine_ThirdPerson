// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/AbstractWeapon.h"
#include "Weapon/BulletBase.h"
#include "Character/CharacterBase.h"
#include "Component/CharacterPickupComponent.h"
#include "Character/CharacterModel.h"
#include "Player/MockCharacter.h"
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
	SkeletalMeshComponent->SetSimulatePhysics(true);
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
	if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
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
	if (WeaponItemInfo.MaxAmmo <= NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
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
		return;
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

void AAbstractWeapon::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AAbstractWeapon::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor))
	{
		if (!IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
		{
			return;
		}
	}

	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherActor))
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

	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherActor))
	{
		UpdateCustomDepth(false);
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

void AAbstractWeapon::TakeDamageOuter(const FHitResult& HitResult, const float AdditionalDamage)
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
	IDamageInstigator::Execute_MakeDamage(GetPawnOwner(), DamageModel, AdditionalDamage, TotalDamage);

	// @NOTE
	// Head Shot Overkill
	if (IDamageInstigator::Execute_CanKillDealDamage(OtherActor, HitResult.BoneName))
	{
		TotalDamage += DamageModel->GetHealth();
	}
	// @TODO
	TSubclassOf<UDamageType> DamageType;
	UGameplayStatics::ApplyPointDamage(
		OtherActor, 
		TotalDamage, 
		GetActorLocation(), 
		HitResult,
		GetPointer()->GetController(), 
		this, 
		DamageType);
}

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
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos"));
		DoReload_Implementation();
		return;
	}

	const FVector StartLocation = CharacterPtr.Get()->BulletTraceRelativeLocation();
	const FVector ForwardLocation = CharacterPtr.Get()->BulletTraceForwardLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName("");
	CollisionQueryParams.OwnerTag = FName("");
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
	ISoundInstigator::Execute_ReportNoiseOther(CharacterPtr.Get(), this, FireSound, DEFAULT_VOLUME, MuzzleLocation);
	CharacterPtr.Get()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	ISoundInstigator::Execute_ReportNoiseOther(CharacterPtr.Get(), this, ImpactSound, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	if (bDebugTrace)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartPoint, EndPoint, FLinearColor::Red, 2.0f, 2.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletsTemplate, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	bool bCanDamageResult = false;
	CheckIfValid(HitData.GetActor(), bCanDamageResult);
	if (bCanDamageResult)
	{
		TakeDamageOuter(HitData, WeaponItemInfo.Damage);
	}
	PlayEffect(HitData, SkeletalMeshComponent);
}

void AAbstractWeapon::OnFireReleaseInternal()
{
	//
}

void AAbstractWeapon::ReloadBulletParams()
{
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);

	if (WeaponItemInfo.MaxAmmo <= NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
}

void AAbstractWeapon::SetEquip(const bool InEquip)
{
	bEquip = InEquip;
}

void AAbstractWeapon::SetReload(const bool InReload)
{
	bReload = InReload;
}

void AAbstractWeapon::PrepareDestroy()
{
	UWorld* const World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
}

void AAbstractWeapon::PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent)
{
	FTransform EmitterTransform;
	EmitterTransform.SetIdentity();
	EmitterTransform.SetLocation(HitResult.Location);

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ImpactEmitterTemplate,
		EmitterTransform,
		true);

	UGameplayStatics::SpawnEmitterAttached(
		FlashEmitterTemplate,
		InComponent,
		MuzzleSocketName,
		GetMuzzleTransform().GetLocation(),
		FRotator(GetMuzzleTransform().GetRotation()),
		EAttachLocation::KeepWorldPosition,
		true);
}

FTransform AAbstractWeapon::GetMuzzleTransform() const
{
	if (SkeletalMeshComponent)
	{
		return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
	}
	return FTransform::Identity;
}

bool AAbstractWeapon::WasEquip() const
{
	return bEquip;
}

bool AAbstractWeapon::WasReload() const
{
	return bReload;
}

bool AAbstractWeapon::WasSameWeaponType(EWeaponItemType InWeaponItemType) const
{
	if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
	{
		return false;
	}
	return WeaponItemInfo.WeaponItemType == InWeaponItemType;
}

void AAbstractWeapon::CopyWeaponItemInfo(const FWeaponItemInfo RefWeaponItemInfo)
{
	WeaponItemInfo.UnEquipSocketName = RefWeaponItemInfo.UnEquipSocketName;
	WeaponItemInfo.EquipSocketName = RefWeaponItemInfo.EquipSocketName;
	WeaponItemInfo.WeaponItemType = RefWeaponItemInfo.WeaponItemType;
	WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo.CurrentAmmo;
	WeaponItemInfo.ClipType = RefWeaponItemInfo.ClipType;
	WeaponItemInfo.MaxAmmo = RefWeaponItemInfo.MaxAmmo;
	WeaponItemInfo.Damage = RefWeaponItemInfo.Damage;
	WeaponItemInfo.Texture = RefWeaponItemInfo.Texture;
}

void AAbstractWeapon::UpdateCustomDepth(const bool bEnable)
{
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetRenderCustomDepth(bEnable);
		SkeletalMeshComponent->CustomDepthStencilValue = bEnable ? 1 : 0;
	}
}

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
