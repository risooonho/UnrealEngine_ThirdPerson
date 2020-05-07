﻿// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	UMGManager(nullptr)
{
	ViewPitchMin = -50.f;
	ViewPitchMax = 50.f;
}

void AMockPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = Wevet::ControllerExtension::GetCameraManager(this))
	{
		CameraManager->ViewPitchMin = ViewPitchMin;
		CameraManager->ViewPitchMax = ViewPitchMax;
	}
}

void AMockPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CharacterOwner = Cast<AMockCharacter>(InPawn);
	Initializer();
}

void AMockPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AMockPlayerController::Initializer()
{
	if (UMGManagerClass)
	{
		UMGManager = CreateWidget<UUMGManager>(this, UMGManagerClass);
	}

	if (UMGManager && CharacterOwner)
	{
		UMGManager->Initializer(CharacterOwner);
		UMGManager->AddToViewport();
	}
}

UUMGManager* AMockPlayerController::GetPlayerHUD() const
{
	return UMGManager;
}
