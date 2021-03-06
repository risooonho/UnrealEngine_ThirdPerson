// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Structs/WeaponItemInfo.h"
#include "WeaponInstigator.generated.h"


/*
*	Weapon Interface
*/
UINTERFACE(BlueprintType)
class WEVET_API UWeaponInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IWeaponInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoFirePressed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoFireRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoReplenishment(const FWeaponItemInfo& RefWeaponItemInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	bool CanMeleeStrike() const;
};
