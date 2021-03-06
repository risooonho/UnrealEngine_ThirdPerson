// Copyright © 2018 wevet works All Rights Reserved.


#include "FullbodyAnimInstance.h"
#include "RTIK.h"
#include "Kismet/KismetMathLibrary.h"


UFullbodyAnimInstance::UFullbodyAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxPelvisAdjustHeight = 60.f;
	LeftArmIKAlpha = 0.0f;
	RightArmIKAlpha = 0.0f;
	IKTargetInterpolationSpeed = 10.f;

	LeftFootIKMode = EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
	RightFootIKMode = EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
	ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled;

	IKLeftLegTarget = FTransform::Identity;
	IKRightLegTarget = FTransform::Identity;
	IKLeftHandTarget = FTransform::Identity;
	IKRightHandTarget = FTransform::Identity;
	IKLeftHandTargetInterpolated = FTransform::Identity;
	IKRightHandTargetInterpolated = FTransform::Identity;

	{
		FBoneReference RefBone(FName(TEXT("pelvis")));
		PelvisBone.BoneRef = RefBone;
	}

#if false
	{
		FBoneReference RefBone1(FName(TEXT("upperarm_r")));
		FIKBone Bone1;
		Bone1.BoneRef = RefBone1;
		RightArmChain.BonesRootToEffector.Add(Bone1);

		FBoneReference RefBone2(FName(TEXT("lowerarm_r")));
		FIKBone Bone2;
		Bone2.BoneRef = RefBone2;
		RightArmChain.BonesRootToEffector.Add(Bone2);

		FBoneReference RefBone3(FName(TEXT("hand_r")));
		FIKBone Bone3;
		Bone3.BoneRef = RefBone3;
		RightArmChain.BonesRootToEffector.Add(Bone3);
	}
#endif

}

void UFullbodyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Legs Initialize
	LeftLegWrapper = NewObject<UHumanoidLegChain_Wrapper>(GetOwningActor());
	LeftLegWrapper->Initialize(LeftLeg);
	RightLegWrapper = NewObject<UHumanoidLegChain_Wrapper>(GetOwningActor());
	RightLegWrapper->Initialize(RightLeg);

	// PelvisInitialize
	PelvisBoneWrapper = NewObject<UIKBoneWrapper>(GetOwningActor());
	PelvisBoneWrapper->Initialize(PelvisBone);

	// TraceData Create
	LeftTraceDataWrapper = NewObject<UHumanoidIKTraceData_Wrapper>(GetOwningActor());
	RightTraceDataWrapper = NewObject<UHumanoidIKTraceData_Wrapper>(GetOwningActor());

	// ArmChain Initialize
	LeftArmChainWrapper = NewObject<URangeLimitedIKChainWrapper>(GetOwningActor());
	LeftArmChainWrapper->Initialize(LeftArmChain);
	RightArmChainWrapper = NewObject<URangeLimitedIKChainWrapper>(GetOwningActor());
	RightArmChainWrapper->Initialize(RightArmChain);
}

void UFullbodyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}

void UFullbodyAnimInstance::SetIKLegTarget(class AActor* InIKTarget, FTransform& OutIKTarget)
{
	if (!InIKTarget)
	{
		return;
	}
	FTransform Transform = FTransform::Identity;
	Transform.SetLocation(InIKTarget->GetActorLocation());
	OutIKTarget = Transform;
}

void UFullbodyAnimInstance::SetIKHandTarget(class AActor* InIKTarget, FTransform& OutIKTarget, FTransform& OutIKTargetInterpolated)
{
	if (!InIKTarget)
	{
		return;
	}
	{
		FTransform Transform = FTransform::Identity;
		Transform.SetLocation(InIKTarget->GetActorLocation());
		Transform.SetRotation(InIKTarget->GetActorRotation().Quaternion());
		OutIKTarget = Transform;
	}

	const FVector Current = OutIKTargetInterpolated.GetLocation();
	const FVector Target = InIKTarget->GetActorLocation();
	const FVector Value = UKismetMathLibrary::VInterpTo(Current, Target, GetWorld()->DeltaTimeSeconds, IKTargetInterpolationSpeed);

	FTransform Result = FTransform::Identity;
	Result.SetLocation(Value);
	Result.SetRotation(OutIKTarget.GetRotation());
	OutIKTargetInterpolated = Result;
}

void UFullbodyAnimInstance::SetOntoTarget(const bool InOntoTargetEnabled, EHumanoidLegIKMode& OutLegIKMode)
{
	OutLegIKMode = InOntoTargetEnabled ? EHumanoidLegIKMode::IK_Human_Leg_WorldLocation : EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
}

void UFullbodyAnimInstance::SetArmTorsoIKMode(const bool InIKLeftHandOntoTarget, const bool InIKRightHandOntoTarget)
{
	if (InIKLeftHandOntoTarget && InIKRightHandOntoTarget)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_BothArms;
		LeftArmIKAlpha = 1.0f;
		RightArmIKAlpha = 1.0f;
	}
	else if (InIKLeftHandOntoTarget && !InIKRightHandOntoTarget)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_LeftArmOnly;
		LeftArmIKAlpha = 1.0f;
		RightArmIKAlpha = 0.0f;
	}
	else if (!InIKLeftHandOntoTarget && InIKRightHandOntoTarget)
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_RightArmOnly;
		LeftArmIKAlpha = 0.0f;
		RightArmIKAlpha = 1.0f;
	}
	else
	{
		ArmTorsoIKMode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled;
		LeftArmIKAlpha = 0.0f;
		RightArmIKAlpha = 0.0f;
	}
}
