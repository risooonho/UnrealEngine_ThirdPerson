// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MockCharacter.h"
#include "MainUIController.generated.h"

class UProgressBar;
class UCanvasPanel;
class UUniformGridPanel;
class UImage;
class UText;
class UHorizontalBox;

UCLASS()
class WEVET_API UMainUIController : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainUIController(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

//protected:
//	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void Initializer(ACharacterBase* NewCharacter);
	void Renderer(float InDeltaTime);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UMainUIController|Variable")
	class ACharacterBase* CharacterOwner;

	class UImage* RadialProgressImage;
	class UCanvasPanel* BasePanel;
	class UCanvasPanel* FocusPanel;
	class UUniformGridPanel* WeaponGridPanel;
	class UImage* WeaponItemImage;
	class UHorizontalBox* AmmoCounterBox;
	class UTextBlock* CurrentAmmoText;
	class UTextBlock* MaxAmmoText;
	class UTextBlock* SpaceAmmoText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName RadialProgressImageKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName BasePanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName FocusPanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName WeaponGridPanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName WeaponItemImageKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName CurrentAmmoKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName MaxAmmoKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName SpaceAmmoKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|Variable")
	FName AmmoCounterBoxKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMainUIController|MaterialParameter")
	FName HealthScalarParameterValueName;

	virtual void SetHealth();
	virtual void SetVisibilityWeapon();
	virtual void SetVisibilityWeaponDetail();
	bool bHasWeapon;
	
};
