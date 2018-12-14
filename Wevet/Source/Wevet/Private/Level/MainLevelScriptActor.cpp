// Copyright © 2018 wevet works All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MockCharacter.h"
#include "Engine.h"


AMainLevelScriptActor::AMainLevelScriptActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
}
