// Copyright © 2018 wevet works All Rights Reserved.

#include "Widget/MainUIController.h"
#include "Engine.h"


UMainUIController::UMainUIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterOwner = nullptr;
	BasePanel = nullptr;
	PlayerHealth = nullptr;
	WeaponFocus = nullptr;

	bWasEquipWeapon = false;
	bTickEventWhenPaused = false;

	/* RootWidget */
	BasePanelKeyName = (TEXT("BasePanel"));
	
	/* PlayerHealth */
	PlayerHealthKeyName = (TEXT("PlayerHealth"));

	/* WeaponFocus */
	WeaponFocusKeyName = (TEXT("WeaponFocus"));

	// @TODO
	/* WeaponWindow */
	WeaponWindowKeyName = (TEXT("WeaponWindow"));
}

void UMainUIController::NativeConstruct()
{
	Super::NativeConstruct();

	BasePanel = Cast<UCanvasPanel>(GetWidgetFromName(BasePanelKeyName));
	WeaponFocus = Cast<UWeaponFocus>(GetWidgetFromName(WeaponFocusKeyName));
	WeaponWindow = Cast<UWeaponWindow>(GetWidgetFromName(WeaponWindowKeyName));

	if (WeaponFocus)
	{
		WeaponFocus->Visibility(false);
	}

	if (WeaponWindow)
	{
		WeaponWindow->Visibility(false);
	}
}

void UMainUIController::Initializer(ACharacterBase* const NewCharacter)
{
	CharacterOwner = NewCharacter;
	PlayerHealth = Cast<UPlayerHealth>(GetWidgetFromName(PlayerHealthKeyName));

	if (PlayerHealth)
	{
		PlayerHealth->Initializer(CharacterOwner);
	}
}

void UMainUIController::Renderer(float InDeltaTime)
{
	if (CharacterOwner)
	{
		ToggleFocus();
	}

	if (PlayerHealth)
	{
		PlayerHealth->Renderer(InDeltaTime);
	}

	if (bWasEquipWeapon)
	{
		if (WeaponWindow)
		{
			WeaponWindow->RendererAmmos(CharacterOwner->GetSelectedWeapon());
		}
	}
}

void UMainUIController::ToggleFocus()
{
	const bool bWeapon = (CharacterOwner->GetSelectedWeapon() != nullptr);
	if (bWeapon == bWasEquipWeapon)
	{
		return;
	}

	bWasEquipWeapon = bWeapon;
	if (WeaponFocus)
	{
		WeaponFocus->Visibility(bWasEquipWeapon);
	}

	if (WeaponWindow)
	{
		WeaponWindow->RendererImage(CharacterOwner->GetSelectedWeapon());
		WeaponWindow->Visibility(bWasEquipWeapon);
	}
}

void UMainUIController::SetTickableWhenPaused()
{
	UWorld* const World = GEngine->GameViewport->GetWorld();
	if (World)
	{
		bTickEventWhenPaused = !bTickEventWhenPaused;
		UGameplayStatics::SetGamePaused(World, bTickEventWhenPaused);
		UE_LOG(LogWevetClient, Log, TEXT("bTickEventWhenPaused : %s"), bTickEventWhenPaused ? TEXT("true") : TEXT("false"));
	}
}
