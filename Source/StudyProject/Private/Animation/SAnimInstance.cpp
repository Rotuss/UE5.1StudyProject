// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SAnimInstance.h"
#include "Character/SViewCharacter.h"
#include "Component/SStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USAnimInstance::USAnimInstance()
{
}

// 초기화 작업
void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CurrentSpeed = 0.0f;

	Velocity = FVector::ZeroVector;

	bIsFalling = false;
	bIsCrouching = false;
	bIsDead = false;
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 왜 TryGetPawnOwner() 함수에 Try-가 붙을까? 
	// TryGetPawnOwner() 함수와 반대로, 폰에서 애님인스턴스를 가져올 때는 GetAnimInstance() 함수
	// 인풋을 받아야 게임 로직이 돌아감 그리고 나서 애니메이션이 돌아감
	// 폰에서는 애니메이션을 갖고 있지만 애니메이션 쪽에서 폰이 없을 수도 있음
	// 예를 들어서 폰이 움직이다가 어떤 경우에 의해 죽었을 때 애니메이션 쪽에선 폰이 없는거나 마찬가지
	// 그런 경우 nullptr을 가져오기 때문에 이런 위험을 방지하고자 애니메이션 쪽에서는 Try를 붙임
	ASCharacter* OwnerCharacter = Cast<ASCharacter>(TryGetPawnOwner());
	if (true == IsValid(OwnerCharacter))
	{
		UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		if (true == IsValid(CharacterMovementComponent))
		{
			//CurrentSpeed = CharacterMovementComponent->GetLastUpdateVelocity().Size();

			Velocity = CharacterMovementComponent->GetLastUpdateVelocity();
			CurrentSpeed = Velocity.Size();
			bIsFalling = CharacterMovementComponent->IsFalling();
			bIsCrouching = CharacterMovementComponent->IsCrouching();
			//bIsDead = OwnerCharacter->IsDead();
			bIsDead = KINDA_SMALL_NUMBER >= OwnerCharacter->GetStatComponent()->GetCurrentHP();
			Acceleration = CharacterMovementComponent->GetCurrentAcceleration();

			// 가속도와 속도가 0에 가까운 수치면 Idle, 아니면 Walk
			if (KINDA_SMALL_NUMBER > Acceleration.Length() && KINDA_SMALL_NUMBER > Velocity.Length())
			{
				LocomotionState = ELocomotionState::Idle;
			}
			else
			{
				LocomotionState = ELocomotionState::Walk;
			}

			ASViewCharacter* OwnerPlayerCharacter = Cast<ASViewCharacter>(OwnerCharacter);
			if (true == IsValid(OwnerPlayerCharacter))
			{
				// 포워드 양수: 앞
				if (KINDA_SMALL_NUMBER < OwnerPlayerCharacter->GetForwardInputValue()) MovementDirection = EMovementDirection::Fwd;
				// 포워드 음수: 뒤
				if (-KINDA_SMALL_NUMBER > OwnerPlayerCharacter->GetForwardInputValue()) MovementDirection = EMovementDirection::Bwd;
				// 우측 양수: 오
				if (KINDA_SMALL_NUMBER < OwnerPlayerCharacter->GetRightInputValue()) MovementDirection = EMovementDirection::Right;
				// 우측 음수: 왼
				if (-KINDA_SMALL_NUMBER > OwnerPlayerCharacter->GetRightInputValue()) MovementDirection = EMovementDirection::Left;

				// 방향 가져오기
				ControlRotation.Pitch = OwnerPlayerCharacter->GetCurrentAimPitch();
				ControlRotation.Yaw = OwnerPlayerCharacter->GetCurrentAimYaw();
			}
		}
	}
}

void USAnimInstance::PlayAnimMontage(UAnimMontage* InAnimMontage)
{
	checkf(true == IsValid(InAnimMontage), TEXT("Invalid InAnimMontage"));

	// 애니메이션이 실행중이 아닐때만
	if (false == Montage_IsPlaying(InAnimMontage))
	{
		Montage_Play(InAnimMontage);
	}
}

void USAnimInstance::AnimNotify_CheckHit()
{
	// 충돌 검사
	if (true == OnCheckHit.IsBound())
	{
		// 해당 델리게이트에 1개의 함수라도 바인드 되어 있다면 true를 반환하는 IsBound() 함수
		OnCheckHit.Broadcast();
	}
}

void USAnimInstance::AnimNotify_CheckAttackInput()
{
	// 아무것도 바인드가 안되어 있는데 브로드캐스트하는 것은 무의밈하므로 한 번 체크하여 작업
	if (true == OnCheckAttackInput.IsBound())
	{
		OnCheckAttackInput.Broadcast();
	}
}
