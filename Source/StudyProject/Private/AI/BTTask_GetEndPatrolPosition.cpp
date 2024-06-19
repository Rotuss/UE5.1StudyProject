// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetEndPatrolPosition.h"
#include "Controller/SAIController.h"
#include "Character/SNonViewCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	// Behavior Tree에 보일 노드 이름
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

	// ASAIController에서 지운 ASAIController::OnPatrolTimerElapsed() 로직과 같은 기능
	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ASAIController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	// 현재 위치(StartPatrolPosition)에서 반경(AIController->PatrolRadius) 이동 할 것인지 다음 위치로 지정(EndPatrolLocation 채워줌)
	if (true == NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius, EndPatrolLocation))
	{
		// EndPatrolPositionKey를 EndPatrolLocation.Location으로 설정
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		
		// 노드 성공 반환
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;
}
