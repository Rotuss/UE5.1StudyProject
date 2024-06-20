// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNonViewCharacter.h"
#include "Controller/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/SAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Character/SViewCharacter.h"

ASNonViewCharacter::ASNonViewCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ASAIController::StaticClass();
	// ASNonViewCharacter는 레벨에 배치되거나 새롭게 생성되면 SAIController의 빙의가 자동으로 진행
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ASNonViewCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);

		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

float ASNonViewCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Super::TakeDamage()에서 나머지 처리
	if (KINDA_SMALL_NUMBER > CurrentHP)
	{
		ASAIController* AIController = Cast<ASAIController>(GetController());
		if (true == IsValid(AIController))
		{
			AIController->EndAI();
		}

		// 죽인 액터가 ASViewCharacter라면 킬 카운트 증가
		ASViewCharacter* DamageCauserCharacter = Cast<ASViewCharacter>(DamageCauser);
		if (true == IsValid(DamageCauserCharacter))
		{
			DamageCauserCharacter->AddCurrentKillCount(1);
		}

		Destroy();
	}

	return FinalDamageAmount;
}

void ASNonViewCharacter::BeginAttack()
{
	// 애님 인스턴스 가져오기
	USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	// 움직임 멈춤
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// 공격 중으로 변환
	bIsNowAttacking = true;
	// 몽타주 실행
	AnimInstance->PlayAnimMontage(FireAttackMontage);

	// OnMeleeAttackMontageEndedDelegate 바인드 된 것이 없다면 바인드 설정
	if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, FireAttackMontage);
	}

}

void ASNonViewCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	// 걷기 가능
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// 공격 중이 아닌 것으로 변환
	bIsNowAttacking = false;

	// 바인드 되어 있다면 해제 작업
	if (true == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}
