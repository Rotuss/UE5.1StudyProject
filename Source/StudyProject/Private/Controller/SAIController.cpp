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

	// PatrolTimerHandle 타이머 핸들을 이용하여
	// PatrolRepeatInterval초 마다 &ThisClass::OnPatrolTimerElapsed 함수를 반복 호출(true니까)
	GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &ThisClass::OnPatrolTimerElapsed, PatrolRepeatInterval, true);
}

void ASAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 핸들 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void ASAIController::OnPatrolTimerElapsed()
{
	// GetPawn(): AIController가 가지고 있는(빙의한) Pawn을 가져옴
	APawn* ControlledPawn = GetPawn();
	if (true == IsValid(ControlledPawn))
	{
		// 네비게이션 시스템을 가져옴
		// 네비게이션 시스템은 해당 월드에 있는 싱글톤 개체라고 보면 됨
		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (true == IsValid(NavigationSystem))
		{
			// 현재 위치
			FVector ActorLocation = ControlledPawn->GetActorLocation();
			// 다음으로 이동할 위치
			FNavLocation NextLocation;
			// 네비게이션 시스템에 의해 GetRandomPointInNavigableRadius함수 사용
			// GetRandomPointInNavigableRadius: 현재 위치(ActorLocation)에서 반경(PatrolRadius) 이동 할 것인지 다음 위치로 지정(NextLocation 채워줌)
			if (true == NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation, PatrolRadius, NextLocation))
			{
				// 컨트롤러를 다음 위치로 이동시켜주는 함수
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
			}
		}
	}
}
