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

	// AIController ��������
	ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetAIOwner());
	if (true == IsValid(AIC))
	{
		// AIC�� Pawn ��������
		ASNonViewCharacter* NPC = Cast<ASNonViewCharacter>(AIC->GetPawn());
		if (true == IsValid(NPC))
		{
			UWorld* World = NPC->GetWorld();
			if (true == IsValid(World))
			{
				// ���� NPC�� ��ġ
				FVector CenterPosition = NPC->GetActorLocation();
				// Ž�� ����
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

				// Ž�� ������ �ƴٸ�
				if (true == bResult)
				{
					for (auto const& OverlapResult : OverlapResults)
					{
						// Ž���� ���߿� �ش� ���Ͱ� ASCharacter�� ������� Ȯ��
						ASCharacter* PC = Cast<ASCharacter>(OverlapResult.GetActor());
						if (true == IsValid(PC))
						{
							// IsPlayerController�� �÷��̾� ��Ʈ�ѷ��� ���� ���ǵǾ� �ִ��� Ȯ�����ִ� ��
							// �� �÷��̾� ��Ʈ�ѷ��� ���ǵǾ� �ִ� ��쿡 �Ʒ��� ���� �����ض�
							if (true == PC->GetController()->IsPlayerController())
							{
								// ������ �� TargetActorKey�� �÷��̾� ��Ʈ�ѷ� ���� ����
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
								// ������ �� TargetActorKey�� nullptr ����
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
					// ������ �� TargetActorKey�� nullptr ����
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
