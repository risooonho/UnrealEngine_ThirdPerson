// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/SniperRifle.h"
#include "Weapon/BulletBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Character/CharacterBase.h"

ASniperRifle::ASniperRifle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BulletDuration = 0.6f;
	CollisionComponent->SetBoxExtent(FVector(16.f, 80.f, 32.f));

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Sniper_Rifle_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/SniperRifle/Cues/SniperRifleA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/SniperRifle/Cues/SniperRifle_ImpactSurface_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_SniperRifle_MuzzleFlash_01"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Large_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Bullet/BP_SniperRifle_Bullet.BP_SniperRifle_Bullet_C"));
		BulletsTemplate = FindAsset.Object;
	}

	// SVD
	NeededAmmo = 2;
	WeaponItemInfo.MaxAmmo = 40;
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Sniper_Rifle_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Sniper;
	WeaponItemInfo.DisplayName = FString(TEXT("SVD"));
}
