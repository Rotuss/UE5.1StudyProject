// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNonViewCharacter.h"
#include "Controller/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
