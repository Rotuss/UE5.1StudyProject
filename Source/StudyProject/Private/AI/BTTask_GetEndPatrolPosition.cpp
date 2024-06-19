// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetEndPatrolPosition.h"
#include "Controller/SAIController.h"
#include "Character/SNonViewCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	// Behavior Tree�� ���� ��� �̸�
	NodeName = TEXT("GetEndPatrolPosition");
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (EBTNodeResult::Failed == Result) return Result;

	ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner());
	checkf(true == IsValid(AIController), TEXT("Invalid AIController"));

	ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIController->GetPawn());
	checkf(true == IsValid(NPC), TEXT("Invalid NPC"));

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
	checkf(true == IsValid(NavigationSystem), TEXT("Invalid NavigationSystem"));

	// ASAIController���� ���� ASAIController::OnPatrolTimerElapsed() ������ ���� ���
	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ASAIController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	// ���� ��ġ(StartPatrolPosition)���� �ݰ�(AIController->PatrolRadius) �̵� �� ������ ���� ��ġ�� ����(EndPatrolLocation ä����)
	if (true == NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius, EndPatrolLocation))
	{
		// EndPatrolPositionKey�� EndPatrolLocation.Location���� ����
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		
		// ��� ���� ��ȯ
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;
}
