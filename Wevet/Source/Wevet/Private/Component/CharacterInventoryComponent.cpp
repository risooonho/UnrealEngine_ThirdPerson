// Copyright © 2018 wevet works All Rights Reserved.

#include "CharacterInventoryComponent.h"
#include "Weapon/AbstractWeapon.h"
#include "Item/ItemBase.h"
#include "Wevet.h"
#include "WevetExtension.h"

UCharacterInventoryComponent::UCharacterInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	WeaponArray.Reset(0);
	ItemArray.Reset(0);
}

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WeaponArray.Reset(0);
	ItemArray.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void UCharacterInventoryComponent::AddWeaponInventory(AAbstractWeapon* const NewWeaponBase)
{
	if (!WeaponArray.Contains(NewWeaponBase))
	{
		WeaponArray.Add(NewWeaponBase);
		WeaponArray.Shrink();
	}
}

void UCharacterInventoryComponent::RemoveWeaponInventory(AAbstractWeapon* const WeaponBase)
{
	if (WeaponArray.Contains(WeaponBase))
	{
		WeaponArray.Remove(WeaponBase);
		WeaponArray.Shrink();
	}
}

void UCharacterInventoryComponent::ClearWeaponInventory()
{
	WeaponArray.Empty();
}

const TArray<class AAbstractWeapon*>& UCharacterInventoryComponent::GetWeaponInventory()
{
	return WeaponArray;
}

bool UCharacterInventoryComponent::HasInventoryWeaponItems() const
{
	return Wevet::ArrayExtension::NullOrEmpty(WeaponArray);
}

void UCharacterInventoryComponent::AddItemInventory(AItemBase* const NewItemBase)
{
	if (!ItemArray.Contains(NewItemBase))
	{
		ItemArray.Add(NewItemBase);
		ItemArray.Shrink();
	}
}

void UCharacterInventoryComponent::RemoveItemInventory(AItemBase* const ItemBase)
{
	if (ItemArray.Contains(ItemBase))
	{
		ItemArray.Remove(ItemBase);
		ItemArray.Shrink();
	}
}

void UCharacterInventoryComponent::ClearItemInventory()
{
	ItemArray.Empty();
}

const TArray<AItemBase*>& UCharacterInventoryComponent::GetItemInventory()
{
	return ItemArray;
}

bool UCharacterInventoryComponent::HasInventoryItems() const
{
	return Wevet::ArrayExtension::NullOrEmpty(ItemArray);
}

void UCharacterInventoryComponent::RemoveAllInventory()
{
	if (!Wevet::ArrayExtension::NullOrEmpty(WeaponArray))
	{
		for (AAbstractWeapon*& Weapon : WeaponArray)
		{
			if (Weapon && Weapon->IsValidLowLevel())
			{
				Weapon->Destroy();
				Weapon->ConditionalBeginDestroy();
				Weapon = nullptr;
			}
		}
		WeaponArray.Empty();
	}

	if (!Wevet::ArrayExtension::NullOrEmpty(ItemArray))
	{
		for (AItemBase*& Item : ItemArray)
		{
			if (Item && Item->IsValidLowLevel())
			{
				Item->Destroy();
				Item->ConditionalBeginDestroy();
				Item = nullptr;
			}
		}
		ItemArray.Empty();
	}
}
