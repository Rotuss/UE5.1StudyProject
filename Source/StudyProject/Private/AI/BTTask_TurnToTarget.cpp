// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SAIController.h"
#include "Character/SCharacter.h"
#include "Character/SViewCharacter.h"
#include "Character/SNonViewCharacter.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTargetActor");

}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner());
	checkf(true == IsValid(AIController), TEXT("Invalid AIController."));

	ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIController->GetPawn());
	checkf(true == IsValid(NPC), TEXT("Invalid NPC."));

	if (ASViewCharacter* TargetPC = Cast<ASViewCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetActorKey)))
	{
		// NPC�κ��� �÷��̾�� ���� ���� ����
		FVector LookVector = TargetPC->GetActorLocation() - NPC->GetActorLocation();
		LookVector.Z = 0.0f;
		// LookVector�� X�� ����
		FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
		// ���� NPC Rotation�� TargetRotation���� ����
		NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.0f));

		return Result = EBTNodeResult::Succeeded;
	}

	return Result = EBTNodeResult::Failed;
}
