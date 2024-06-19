// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNonViewCharacter.h"
#include "Controller/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ASNonViewCharacter::ASNonViewCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ASAIController::StaticClass();
	// ASNonViewCharacter는 레벨에 배치되거나 새롭게 생성되면 SAIController의 빙의가 자동으로 진행
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}
