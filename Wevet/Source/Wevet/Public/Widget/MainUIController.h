// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/MockCharacter.h"
#include "Widget/PlayerHealth.h"
#include "Widget/WeaponFocus.h"
#include "Widget/WeaponWindow.h"
#include "MainUIController.generated.h"


UCLASS()
class WEVET_API UMainUIController : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainUIController(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

public:
	void Initializer(ACharacterBase* const NewCharacter);
	void Renderer(float InDeltaTime);
	void SetTickableWhenPaused();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MainUIController|Variable")
	class ACharacterBase* CharacterOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUIController|Variable")
	FName PlayerHealthKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUIController|Variable")
	FName WeaponFocusKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUIController|Variable")
	FName BasePanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainUIController|Variable")
	FName WeaponWindowKeyName;

	class UPlayerHealth* PlayerHealth;
	class UWeaponFocus* WeaponFocus;
	class UWeaponWindow* WeaponWindow;
	class UCanvasPanel* BasePanel;

protected:
	void ToggleFocus();
	bool bWasEquipWeapon;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Tick", AdvancedDisplay)
	bool bTickEventWhenPaused;
};
