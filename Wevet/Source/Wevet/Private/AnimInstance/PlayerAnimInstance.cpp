// Copyright © 2018 wevet works All Rights Reserved.


#include "AnimInstance/PlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Player = Cast<AMockCharacter>(Owner);
	InitDefaultValue();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
	SetVariableFromOwner();
}

// @NOTE
// Player変数の初期値をcopyする
void UPlayerAnimInstance::InitDefaultValue()
{
	if (!Player)
	{
		return;
	}
	ILocomotionSystemState::Execute_SetALSMovementMode(this, Player->GetALSMovementMode());
	ILocomotionSystemState::Execute_SetALSGait(this, Player->GetALSGait());
	ILocomotionSystemState::Execute_SetALSStance(this, Player->GetALSStance());
	ILocomotionSystemState::Execute_SetALSRotationMode(this, Player->GetALSRotationMode());
	ILocomotionSystemState::Execute_SetALSViewMode(this, Player->GetALSViewMode());
	ILocomotionSystemAction::Execute_SetAiming(this, Player->HasAiming());
	ILocomotionSystemSpeed::Execute_SetWalkingSpeed(this, Player->GetWalkingSpeed());
	ILocomotionSystemSpeed::Execute_SetRunningSpeed(this, Player->GetRunningSpeed());
	ILocomotionSystemSpeed::Execute_SetSprintingSpeed(this, Player->GetSprintingSpeed());
	ILocomotionSystemSpeed::Execute_SetCrouchingSpeed(this, Player->GetCrouchingSpeed());
	ILocomotionSystemState::Execute_SetALSIdleState(this, 
		ALSStance == ELSStance::Standing ? 
		ELSIdleEntryState::N_Idle : ELSIdleEntryState::CLF_Idle);
}

// @NOTE
// Player変数を都度copyする
void UPlayerAnimInstance::SetVariableFromOwner()
{
	if (!Player)
	{
		return;
	}
	CharacterRotation = Player->CharacterRotation;
	LastVelocityRotation = Player->LastVelocityRotation;
	LastMovementInputRotation = Player->LastMovementInputRotation;
	LookingRotation = Player->LookingRotation;
	Direction = Player->Direction;
	RotationDifference = Player->RotationDifference;
	VelocityDifference = Player->VelocityDifference;
	AimYawDelta = Player->AimYawDelta;
	AimYawRate = Player->AimYawRate;
	bWasMoving = Player->HasMoving();
	bWasMovementInput = Player->HasMovementInput();
	Velocity = Player->ChooseVelocity();
}

void UPlayerAnimInstance::SetMovementSpeed()
{
	// @NOTE
	// Not Calling Parent Class
}

#pragma region ALSInterface
void UPlayerAnimInstance::SetALSGait_Implementation(const ELSGait InLSGait)
{
	ALSGait = InLSGait;
	UE_LOG(LogWevetClient, Log, TEXT("Gait : %s, funcName : %s"), *GETENUMSTRING("ELSGait", ALSGait), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSStance_Implementation(const ELSStance InLSStance)
{
	ALSStance = InLSStance;
	OnStanceChanged();
	UE_LOG(LogWevetClient, Log, TEXT("Stance : %s, funcName : %s"), *GETENUMSTRING("ELSStance", ALSStance), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSRotationMode_Implementation(const ELSRotationMode InLSRotationMode)
{
	ALSRotationMode = InLSRotationMode;
	UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s, funcName : %s"), *GETENUMSTRING("ELSRotationMode", ALSRotationMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSMovementMode_Implementation(const ELSMovementMode InLSMovementMode)
{
	ALSPrevMovementMode = ALSMovementMode;
	ALSMovementMode = InLSMovementMode;
	UE_LOG(LogWevetClient, Log, TEXT("Movement : %s, funcName : %s"), *GETENUMSTRING("ELSMovementMode", ALSMovementMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSViewMode_Implementation(const ELSViewMode InLSViewMode)
{
	ALSViewMode = InLSViewMode;
	UE_LOG(LogWevetClient, Log, TEXT("ViewMode : %s, funcName : %s"), *GETENUMSTRING("ELSViewMode", ALSViewMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState)
{
	if (ALSIdleEntryState != InLSIdleEntryState)
	{
		ALSIdleEntryState = InLSIdleEntryState;
		UE_LOG(LogWevetClient, Log, TEXT("Idle : %s, funcName : %s"), *GETENUMSTRING("ELSIdleEntryState", ALSIdleEntryState), *FString(__FUNCTION__));
	}
}

void UPlayerAnimInstance::SetAiming_Implementation(const bool InAiming)
{
	bWasAiming = InAiming;
}

void UPlayerAnimInstance::SetGetup_Implementation(const bool InFaceDown)
{
	UE_LOG(LogWevetClient, Log, TEXT("Getup : %s"), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetRF_Implementation(const bool InRF)
{
	bRF = InRF;
}

void UPlayerAnimInstance::PoseSnapShot_Implementation(const FName InPoseName)
{
	SavePoseSnapshot(InPoseName);
	UE_LOG(LogWevetClient, Log, TEXT("PoseSnapShot : %s, funcName : %s"), *InPoseName.ToString(), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSAnimNotifyTurnInPlace_Implementation(
	UAnimMontage* InTurnInPlaceMontage, 
	const bool InShouldTurnInPlace, 
	const bool InTurningInPlace, 
	const bool InTurningRight)
{
	ActiveTurnInPlaceMontage = InTurnInPlaceMontage;
	bShouldTurnInPlace = InShouldTurnInPlace;
	bTurningInPlace = InTurningInPlace;
	bTurningRight = InTurningRight;

	if (InTurnInPlaceMontage)
	{
		UE_LOG(LogWevetClient, Log, TEXT("TurnInPlace : %s, funcName : %s"), *InTurnInPlaceMontage->GetName(), *FString(__FUNCTION__));
	}
}

void UPlayerAnimInstance::SetALSAnimNotifyPivotData_Implementation(const FPivotData InPivotData)
{
	PivotData = InPivotData;
}
#pragma endregion

#pragma region ALS
void UPlayerAnimInstance::OnStanceChanged()
{
	if (!ActiveTurnInPlaceMontage)
	{
		return;
	}

	if (bTurningInPlace)
	{
		const float BlendOut = 0.3f;
		Montage_Stop(BlendOut, ActiveTurnInPlaceMontage);
	}
}

// @NOTE
// 歩行、走行、および疾走の速度(walk=1、run=2、sprint=3)に関連する現在の速度に基づいて、歩行値(1〜3の範囲)を設定
// BlendSpace内で完全にwait付けされたAnimationを維持しながら、walk/run/sprintの速度を変更
void UPlayerAnimInstance::CalculateGaitValue()
{
	const float MoveSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, 0.0f, WalkingSpeed, 0.0f, 1.0f);
	const float WalkSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 1.0f, 2.0f);
	const float RunSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, RunningSpeed, SprintingSpeed, 2.0f, 3.0f);

	const bool bWalkedGreater = (MovementSpeed > WalkingSpeed);
	const bool bRunnedGreater = (MovementSpeed > RunningSpeed);

	float CurrentSpeed = bWalkedGreater ? WalkSpeed : MoveSpeed;
	CurrentSpeed = bRunnedGreater ? RunSpeed : CurrentSpeed;
	GaitValue = CurrentSpeed;
}

// @NOTE
// 現在のZ速度をCharacterMovementComponent JumpZVelocityと比較
// その値に現在の速度(XとYのみ)を実行速度との関係で掛けて、Air leanを設定。
void UPlayerAnimInstance::CalculateInAirLeaningValues()
{
	const float JumpVelocity = CharacterMovementComponent->JumpZVelocity;
	const float Value = UKismetMathLibrary::MapRangeClamped(Velocity.Z, JumpVelocity, JumpVelocity * -2.f, 1.0f, -1.0f);
	const float NormalizeValue = UKismetMathLibrary::NormalizeToRange(MovementSpeed, 0.0f, RunningSpeed);
	LeanInAir = (Value * NormalizeValue);
}

// @NOTE
// Ragdollの速度に基づいて、FlailRateを増減
void UPlayerAnimInstance::RagdollFailRate()
{
	FlailRate = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(Velocity), 0.0f, 1000.f, 0.0f, 1.25f);
}

// @NOTE
// Falling中は、XとYの速度値のみを使用
// Ground中はX,Y,Zを使用
void UPlayerAnimInstance::UpdateMovementSpeed(const bool bWasGround)
{
	FVector Value = Velocity;
	Value.Z = bWasGround ? Value.Z : 0.0f;
	MovementSpeed = UKismetMathLibrary::VSize(Value);
}

// @NOTE
// Falling中に呼ばれる
void UPlayerAnimInstance::UpdateFlailBlendAlpha()
{
	if (FlailAlphaCurve)
	{
		FlailBlendAlpha = FlailAlphaCurve->GetFloatValue(Velocity.Z * -1.f);
	}
}

void UPlayerAnimInstance::IdleTransition(UAnimSequenceBase* Animation, const float InPlayRate, const float InTimeToStartMontageAt)
{
	if (Animation)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Animation : %s, funcName : %s"), *Animation->GetName(), *FString(__FUNCTION__));

		const FName SlotName(TEXT("Transition"));
		const float BlendInTime = 0.2f;
		const float BlendOutTime = 0.2f;
		const float BlendOutTriggerTime = 0.0f;
		const int32 LoopCount = 1;
		PlaySlotAnimationAsDynamicMontage(Animation, SlotName, BlendInTime, BlendOutTime, InPlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
	}
}

#pragma endregion

