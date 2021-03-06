// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "Player/MockCharacter.h"
#include "Curves/CurveFloat.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemAction.h"
#include "LocomotionSystemNotify.h"
#include "LocomotionSystemState.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UPlayerAnimInstance : public UCharacterAnimInstanceBase, 
	public ILocomotionSystemAction, 
	public ILocomotionSystemNotify, 
	public ILocomotionSystemState
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	virtual void SetMovementSpeed() override;

protected:
	void SetVariableFromOwner();

protected:
	class AMockCharacter* Player;

#pragma region ALSInterface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSGait(const ELSGait InLSGait);
	virtual void SetALSGait_Implementation(const ELSGait InLSGait) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSStance(const ELSStance InLSStance);
	virtual void SetALSStance_Implementation(const ELSStance InLSStance) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSRotationMode(const ELSRotationMode InLSRotationMode);
	virtual void SetALSRotationMode_Implementation(const ELSRotationMode InLSRotationMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSMovementMode(const ELSMovementMode InLSMovementMode);
	virtual void SetALSMovementMode_Implementation(const ELSMovementMode InLSMovementMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSViewMode(const ELSViewMode InLSViewMode);
	virtual void SetALSViewMode_Implementation(const ELSViewMode InLSViewMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_State")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);
	virtual void SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_Action")
	void SetAiming(const bool InAiming);
	virtual void SetAiming_Implementation(const bool InAiming) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_Action")
	void SetGetup(const bool InFaceDown);
	virtual void SetGetup_Implementation(const bool InFaceDown) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_Action")
	void SetRF(const bool InRF);
	virtual void SetRF_Implementation(const bool InRF) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_Action")
	void PoseSnapShot(const FName InPoseName);
	virtual void PoseSnapShot_Implementation(const FName InPoseName) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_AnimNotify")
	void SetALSAnimNotifyTurnInPlace(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight);
	virtual void SetALSAnimNotifyTurnInPlace_Implementation(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnimInstance|ALS_AnimNotify")
	void SetALSAnimNotifyPivotData(const FPivotData InPivotData);
	virtual void SetALSAnimNotifyPivotData_Implementation(const FPivotData InPivotData) override;
#pragma endregion

#pragma region ALS
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSGait ALSGait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSStance ALSStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSRotationMode ALSRotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementMode ALSMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementMode ALSPrevMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSViewMode ALSViewMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSIdleEntryState ALSIdleEntryState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D AimOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D LeanGrounded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D FeetPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator CharacterRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LastVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LastMovementInputRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LookingRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator PreviousVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AimYawDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AimYawRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float GaitValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float N_PlayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float C_PlayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float LeanInAir;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AccelerationDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float PreviousSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float StartPosition;

	// Ragdoll
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float FlailRate;

	// Ragdoll
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float FlailBlendAlpha;

	// Landing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float LandPredictionAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float RotationDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float VelocityDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float TurnInPlaceDelayCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bWasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bTurningInPlace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bTurningRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bShouldTurnInPlace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bRF;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* ActiveTurnInPlaceMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* GetUpFromBack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* GetUpFromFront;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimSequence* AdditiveLand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UCurveFloat* FlailAlphaCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UCurveFloat* LandAlphaCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages N_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages N_Turn_180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages LF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages RF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages CLF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages CRF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FPivotData PivotData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementDirection MovementDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSLocomotionState ActiveLocomotionState;

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS")
	void OnStanceChanged();

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void CalculateGaitValue();

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void CalculateInAirLeaningValues();

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void RagdollFailRate();

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void UpdateMovementSpeed(const bool bWasGround);

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void UpdateFlailBlendAlpha();

	UFUNCTION(BlueprintCallable, Category = "ALS")
	void IdleTransition(UAnimSequenceBase* Animation, const float InPlayRate, const float InTimeToStartMontageAt);

	UFUNCTION(BlueprintCallable, Category = "ALS")
	FTurnMontages GetCrouchTurnData() const
	{
		return bRF ? CRF_Turn_90 : CLF_Turn_90;
	}

	UFUNCTION(BlueprintCallable, Category = "ALS")
	FTurnMontages GetTurnData() const
	{
		return bRF ? RF_Turn_90 : LF_Turn_90;
	}

	void InitDefaultValue();
#pragma endregion

};

