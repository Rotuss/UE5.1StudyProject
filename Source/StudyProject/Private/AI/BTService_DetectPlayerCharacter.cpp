// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_DetectPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/SAIController.h"
#include "Character/SCharacter.h"
#include "Character/SNonViewCharacter.h"

UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("DetectPlayerCharacter");
	Interval = 1.0f;
}

void UBTService_DetectPlayerCharacter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AIController 가져오기
	ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetAIOwner());
	if (true == IsValid(AIC))
	{
		// AIC의 Pawn 가져오기
		ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIC->GetPawn());
		if (true == IsValid(NPC))
		{
			UWorld* World = NPC->GetWorld();
			if (true == IsValid(World))
			{
				// 현재 NPC의 위치
				FVector CenterPosition = NPC->GetActorLocation();
				// 탐색 범위
				float DetectRadius = 300.0f;
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None, false, NPC);
				bool bResult = World->OverlapMultiByChannel(
					OverlapResults,
					CenterPosition,
					FQuat::Identity,
					ECollisionChannel::ECC_GameTraceChannel2,
					FCollisionShape::MakeSphere(DetectRadius),
					CollisionQueryParams
				);

				// 탐지 감지가 됐다면
				if (true == bResult)
				{
					for (auto const& OverlapResult : OverlapResults)
					{
						// 탐지된 값중에 해당 액터가 ASCharacter인 경우인지 확인
						ASCharacter* PC = Cast<ASCharacter>(OverlapResult.GetActor());
						if (true == IsValid(PC))
						{
							// IsPlayerController는 플레이어 컨트롤러에 의해 빙의되어 있는지 확인해주는 것
							// 즉 플레이어 컨트롤러에 빙의되어 있는 경우에 아래와 같이 동작해라
							if (true == PC->GetController()->IsPlayerController())
							{
								// 블랙보드 중 TargetActorKey에 플레이어 컨트롤러 정보 대입
								OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASAIController::TargetActorKey, PC);

								if (1 == ASAIController::ShowAIDebug)
								{
									UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detected!")));
									DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Red, false, 0.5f);
									DrawDebugPoint(World, PC->GetActorLocation(), 10.0f, FColor::Red, false, 0.5f);
									DrawDebugLine(World, NPC->GetActorLocation(), PC->GetActorLocation(), FColor::Red, false, 0.5f, 0u, 3.0f);
								}
							}
							else
							{
								// 블랙보드 중 TargetActorKey에 nullptr 대입
								OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASAIController::TargetActorKey, nullptr);

								if (1 == ASAIController::ShowAIDebug)
								{
									DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
								}
							}
						}
					}
				}
				else
				{
					// 블랙보드 중 TargetActorKey에 nullptr 대입
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASAIController::TargetActorKey, nullptr);
				}

				if (1 == ASAIController::ShowAIDebug)
				{
					DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
				}
			}
		}
	}
}
