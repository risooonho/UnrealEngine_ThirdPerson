// Copyright © 2018 wevet works All Rights Reserved.

#include "Weapon/AIRifle.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"

AAIRifle::AAIRifle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAIRifle::OnFirePressInternal()
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

	const FVector MuzzleLocation = GetMuzzleTransform().GetLocation();
	FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	const FRotator ActorRotation  = GetPointer()->GetActorRotation();
	MuzzleRotation.Yaw = ActorRotation.Yaw;

	const FVector ForwardLocation = MuzzleRotation.Vector();
	const FVector EndLocation = MuzzleLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

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
		MuzzleLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

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
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartPoint, EndPoint, FLinearColor::Red, 2.0f, 2.0f);
	}

	SpawnBullet(Transform);
	OnHitTriggerDelegate(HitData.GetActor(), HitData);
}
