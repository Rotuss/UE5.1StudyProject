// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "Controller/SAIController.h"
#include "Character/SNonViewCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;

}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner());
	checkf(true == IsValid(AIController), TEXT("Invalid AIController."));

	ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIController->GetPawn());
	checkf(true == IsValid(NPC), TEXT("Invalid NPC."));

	// 공격 중인지를 체크
	// 공격이 끝나 공격 중이 아니게되면 (true에서 false로 변경되는 순간)
	if (false == NPC->bIsNowAttacking)
	{
		// 공격 태스크가 성공적으로 끝났음을 알림
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

// 먼저 호출
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner());
	checkf(true == IsValid(AIController), TEXT("Invalid AIController."));

	ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIController->GetPawn());
	checkf(true == IsValid(NPC), TEXT("Invalid NPC."));

	NPC->BeginAttack();

	return EBTNodeResult::InProgress;

}
