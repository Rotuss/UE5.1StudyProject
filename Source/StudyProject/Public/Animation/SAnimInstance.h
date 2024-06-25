// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckAttackInput);

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	None,
	Idle,
	Walk,
	End,

};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	None,
	Fwd,
	Bwd,
	Left,
	Right,
	End,

};

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	USAnimInstance();

	virtual void NativeInitializeAnimation() override;

	// == Tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAnimMontage(class UAnimMontage* InAnimMontage);

	ELocomotionState GetLocomotionState() const { return LocomotionState; }

	EMovementDirection GetMovementDirection() const { return MovementDirection; }

protected:
	UFUNCTION()
	void AnimNotify_CheckHit();

	UFUNCTION()
	void AnimNotify_CheckAttackInput();

public:
	FOnCheckHit OnCheckHit;
	FOnCheckAttackInput OnCheckAttackInput;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	FVector Velocity;

	// ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsFalling : 1;

	// �ɱ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsCrouching : 1;

	// ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	uint8 bIsDead : 1;

	// ĳ���� ���ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAnimInstance", meta = (AllowPrivateAccess))
	FVector Acceleration;

	// ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ELocomotionState LocomotionState;

	// ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMovementDirection MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USTPSAnimInstance", meta = (AllowPrivateAccess = true))
	FRotator ControlRotation;

};
