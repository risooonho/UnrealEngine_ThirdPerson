// Copyright © 2018 wevet works All Rights Reserved.


#include "Widget/WeaponFocus.h"
#include "Components/CanvasPanel.h"

UWeaponFocus::UWeaponFocus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FocusPanelKeyName = FName(TEXT("FocusPanel"));
}

void UWeaponFocus::NativeConstruct()
{
	Super::NativeConstruct();
	FocusPanel = Cast<UCanvasPanel>(GetWidgetFromName(FocusPanelKeyName));
}

void UWeaponFocus::Visibility(const bool InVisibility)
{
	if (FocusPanel)
	{
		FocusPanel->SetVisibility(InVisibility ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}
