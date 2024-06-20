// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNonViewCharacter.h"
#include "Controller/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/SAnimInstance.h"

ASNonViewCharacter::ASNonViewCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ASAIController::StaticClass();
	// ASNonViewCharacter�� ������ ��ġ�ǰų� ���Ӱ� �����Ǹ� SAIController�� ���ǰ� �ڵ����� ����
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

void ASNonViewCharacter::BeginAttack()
{
	// �ִ� �ν��Ͻ� ��������
	USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	// ������ ����
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// ���� ������ ��ȯ
	bIsNowAttacking = true;
	// ��Ÿ�� ����
	AnimInstance->PlayAnimMontage(FireAttackMontage);

	// OnMeleeAttackMontageEndedDelegate ���ε� �� ���� ���ٸ� ���ε� ����
	if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, FireAttackMontage);
	}

}

void ASNonViewCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	// �ȱ� ����
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// ���� ���� �ƴ� ������ ��ȯ
	bIsNowAttacking = false;

	// ���ε� �Ǿ� �ִٸ� ���� �۾�
	if (true == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}
