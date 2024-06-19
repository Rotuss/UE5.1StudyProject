// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

const float ASAIController::PatrolRepeatInterval(3.0f);
const float ASAIController::PatrolRadius(500.0f);

ASAIController::ASAIController()
{
}

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	// PatrolTimerHandle Ÿ�̸� �ڵ��� �̿��Ͽ�
	// PatrolRepeatInterval�� ���� &ThisClass::OnPatrolTimerElapsed �Լ��� �ݺ� ȣ��(true�ϱ�)
	GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &ThisClass::OnPatrolTimerElapsed, PatrolRepeatInterval, true);
}

void ASAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// �ڵ� Ÿ�̸� ����
	GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void ASAIController::OnPatrolTimerElapsed()
{
	// GetPawn(): AIController�� ������ �ִ�(������) Pawn�� ������
	APawn* ControlledPawn = GetPawn();
	if (true == IsValid(ControlledPawn))
	{
		// �׺���̼� �ý����� ������
		// �׺���̼� �ý����� �ش� ���忡 �ִ� �̱��� ��ü��� ���� ��
		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (true == IsValid(NavigationSystem))
		{
			// ���� ��ġ
			FVector ActorLocation = ControlledPawn->GetActorLocation();
			// �������� �̵��� ��ġ
			FNavLocation NextLocation;
			// �׺���̼� �ý��ۿ� ���� GetRandomPointInNavigableRadius�Լ� ���
			// GetRandomPointInNavigableRadius: ���� ��ġ(ActorLocation)���� �ݰ�(PatrolRadius) �̵� �� ������ ���� ��ġ�� ����(NextLocation ä����)
			if (true == NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation, PatrolRadius, NextLocation))
			{
				// ��Ʈ�ѷ��� ���� ��ġ�� �̵������ִ� �Լ�
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
			}
		}
	}
}
