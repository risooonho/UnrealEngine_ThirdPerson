// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraShake.h"
#include "LocomotionSystemPawn.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemPawn : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemPawn
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UCameraShake> InShakeClass, const float InScale);
};
