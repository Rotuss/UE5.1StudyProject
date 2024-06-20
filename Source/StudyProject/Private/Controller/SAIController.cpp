// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

//const float ASAIController::PatrolRepeatInterval(3.0f);
const float ASAIController::PatrolRadius(500.0f);
const FName ASAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ASAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName ASAIController::TargetActorKey(TEXT("TargetActor"));
int32 ASAIController::ShowAIDebug(0);
FAutoConsoleVariableRef CVarShowAIDebug(TEXT("StudyProject.ShowAIDebug"), ASAIController::ShowAIDebug, TEXT(""), ECVF_Cheat);

ASAIController::ASAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	// BrainComponent: AIController Ŭ������ ���ǵ� �Ӽ�
	// UBehaviorTreeComponent�� UBrainComponent�� ��� ����
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	// PatrolTimerHandle Ÿ�̸� �ڵ��� �̿��Ͽ�
	// PatrolRepeatInterval�� ���� &ThisClass::OnPatrolTimerElapsed �Լ��� �ݺ� ȣ��(true�ϱ�)
	//GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &ThisClass::OnPatrolTimerElapsed, PatrolRepeatInterval, true);

	APawn* ControlledPawn = GetPawn();
	if (true == IsValid(ControlledPawn)) BeginAI(ControlledPawn);

}

void ASAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// �ڵ� Ÿ�̸� ����
	//GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);
	
	EndAI();

	Super::EndPlay(EndPlayReason);
}

void ASAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (true == IsValid(BlackboardComponent))
	{
		// BlackboardComponent�� BlackboardDataAsset�� ����ϰڴ�
		if (true == UseBlackboard(BlackboardDataAsset, BlackboardComponent))
		{
			// BehaviorTree ���� ����
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(true == bRunSucceeded, TEXT("Fail to run behavior tree."));

			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey, InPawn->GetActorLocation());

			if (1 == ShowAIDebug)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI() has been called.")));
			}
		}
	}
}

void ASAIController::EndAI()
{
	// �ٿ� ĳ�����Ͽ�
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	// �ٿ� ĳ������ ���� ��ȿ�ϴٸ�
	if (true == IsValid(BehaviorTreeComponent))
	{
		// �����̺�� Ʈ�� ���� ����
		BehaviorTreeComponent->StopTree();

		if (1 == ShowAIDebug)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI() has been called.")));
		}
	}
}

//void ASAIController::OnPatrolTimerElapsed()
//{
//	// GetPawn(): AIController�� ������ �ִ�(������) Pawn�� ������
//	APawn* ControlledPawn = GetPawn();
//	if (true == IsValid(ControlledPawn))
//	{
//		// �׺���̼� �ý����� ������
//		// �׺���̼� �ý����� �ش� ���忡 �ִ� �̱��� ��ü��� ���� ��
//		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
//		if (true == IsValid(NavigationSystem))
//		{
//			// ���� ��ġ
//			FVector ActorLocation = ControlledPawn->GetActorLocation();
//			// �������� �̵��� ��ġ
//			FNavLocation NextLocation;
//			// �׺���̼� �ý��ۿ� ���� GetRandomPointInNavigableRadius�Լ� ���
//			// GetRandomPointInNavigableRadius: ���� ��ġ(ActorLocation)���� �ݰ�(PatrolRadius) �̵� �� ������ ���� ��ġ�� ����(NextLocation ä����)
//			if (true == NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation, PatrolRadius, NextLocation))
//			{
//				// ��Ʈ�ѷ��� ���� ��ġ�� �̵������ִ� �Լ�
//				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
//			}
//		}
//	}
//}
