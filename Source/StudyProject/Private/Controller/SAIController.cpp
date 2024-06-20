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
	// BrainComponent: AIController 클래스에 정의된 속성
	// UBehaviorTreeComponent는 UBrainComponent을 상속 받음
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	// PatrolTimerHandle 타이머 핸들을 이용하여
	// PatrolRepeatInterval초 마다 &ThisClass::OnPatrolTimerElapsed 함수를 반복 호출(true니까)
	//GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &ThisClass::OnPatrolTimerElapsed, PatrolRepeatInterval, true);

	APawn* ControlledPawn = GetPawn();
	if (true == IsValid(ControlledPawn)) BeginAI(ControlledPawn);

}

void ASAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 핸들 타이머 정리
	//GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);
	
	EndAI();

	Super::EndPlay(EndPlayReason);
}

void ASAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (true == IsValid(BlackboardComponent))
	{
		// BlackboardComponent에 BlackboardDataAsset을 사용하겠다
		if (true == UseBlackboard(BlackboardDataAsset, BlackboardComponent))
		{
			// BehaviorTree 동작 실행
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
	// 다운 캐스팅하여
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	// 다운 캐스팅한 것이 유효하다면
	if (true == IsValid(BehaviorTreeComponent))
	{
		// 비헤이비어 트리 동작 멈춤
		BehaviorTreeComponent->StopTree();

		if (1 == ShowAIDebug)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI() has been called.")));
		}
	}
}

//void ASAIController::OnPatrolTimerElapsed()
//{
//	// GetPawn(): AIController가 가지고 있는(빙의한) Pawn을 가져옴
//	APawn* ControlledPawn = GetPawn();
//	if (true == IsValid(ControlledPawn))
//	{
//		// 네비게이션 시스템을 가져옴
//		// 네비게이션 시스템은 해당 월드에 있는 싱글톤 개체라고 보면 됨
//		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
//		if (true == IsValid(NavigationSystem))
//		{
//			// 현재 위치
//			FVector ActorLocation = ControlledPawn->GetActorLocation();
//			// 다음으로 이동할 위치
//			FNavLocation NextLocation;
//			// 네비게이션 시스템에 의해 GetRandomPointInNavigableRadius함수 사용
//			// GetRandomPointInNavigableRadius: 현재 위치(ActorLocation)에서 반경(PatrolRadius) 이동 할 것인지 다음 위치로 지정(NextLocation 채워줌)
//			if (true == NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation, PatrolRadius, NextLocation))
//			{
//				// 컨트롤러를 다음 위치로 이동시켜주는 함수
//				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
//			}
//		}
//	}
//}
