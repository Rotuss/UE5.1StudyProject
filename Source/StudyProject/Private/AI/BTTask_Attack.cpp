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

	// ���� �������� üũ
	// ������ ���� ���� ���� �ƴϰԵǸ� (true���� false�� ����Ǵ� ����)
	if (false == NPC->bIsNowAttacking)
	{
		// ���� �½�ũ�� ���������� �������� �˸�
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

// ���� ȣ��
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
