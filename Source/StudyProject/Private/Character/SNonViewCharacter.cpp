// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SNonViewCharacter.h"
#include "Controller/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/SAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Character/SViewCharacter.h"
#include "Component/SStatComponent.h"
#include "Component/SWidgetComponent.h"
#include "Game/SPlayerState.h"
#include "UI/SW_HPBar.h"

ASNonViewCharacter::ASNonViewCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	AIControllerClass = ASAIController::StaticClass();
	// ASNonViewCharacter는 레벨에 배치되거나 새롭게 생성되면 SAIController의 빙의가 자동으로 진행
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// 위젯 컴포넌트 생성 및 RootComponent에 부착
	WidgetComponent = CreateDefaultSubobject<USWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	// 위치는 캐릭터 머리위에 보이기 위해 Z축으로부터 100.0f위로 설정
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	// EWidgetSpace::Screen는 Billboard 방식(카메라를 향해 UI가 회전)으로 보이나 플레이어 캐릭터를 가리게 됨. 또한 UI와 멀어져도 동일한 크기가 유지
	// WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// EWidgetSpace::World로 하면 UI가 멀어지면 크기가 작아짐, UI 회전도 안됨
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ASNonViewCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);

		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

float ASNonViewCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Super::TakeDamage()에서 나머지 처리
	//if (KINDA_SMALL_NUMBER > CurrentHP)
	if (KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
	{
		ASAIController* AIController = Cast<ASAIController>(GetController());
		if (true == IsValid(AIController))
		{
			AIController->EndAI();
		}

		// 죽인 액터가 ASViewCharacter라면 킬 카운트 증가
		ASViewCharacter* DamageCauserCharacter = Cast<ASViewCharacter>(DamageCauser);
		if (true == IsValid(DamageCauserCharacter))
		{
			//DamageCauserCharacter->AddCurrentKillCount(1);

			ASPlayerState* SPlayerState = Cast<ASPlayerState>(DamageCauserCharacter->GetPlayerState());
			if (true == IsValid(SPlayerState))
			{
				SPlayerState->AddCurrentKillCount(1);
			}
		}

		Destroy();
	}

	return FinalDamageAmount;
}

void ASNonViewCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (true == IsValid(WidgetComponent))
	{
		// 빌보드를 아예 안 쓸 수는 없음 따라서 현재 내 플레이어를 바라보게 만들어줘야 함

		FVector WidgetComponentLocation = WidgetComponent->GetComponentLocation();
		// UGameplayStatics::GetPlayerCameraManager(this, 0)는 현재 내가 조작하는 플레이어를 뜻함
		FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
		// WidgetComponentLocation에서 LocalPlayerCameraLocation를 바라보는 방향의 회전값을 설정
		// 즉 위젯이 플레이어를 향해 바라보게 됨
		WidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));
	}

}

void ASNonViewCharacter::SetWidget(UStudyWidget* InStudyWidget)
{
	Super::SetWidget(InStudyWidget);

	USW_HPBar* HPBarWidget = Cast<USW_HPBar>(InStudyWidget);
	if (true == IsValid(HPBarWidget))
	{
		// 위젯 초기화
		HPBarWidget->SetMaxHP(StatComponent->GetMaxHP());
		HPBarWidget->InitializeHPBarWidget(StatComponent);
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBarWidget, &USW_HPBar::OnCurrentHPChange);
	}

}

void ASNonViewCharacter::BeginAttack()
{
	// 애님 인스턴스 가져오기
	USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	// 움직임 멈춤
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// 공격 중으로 변환
	bIsNowAttacking = true;
	// 몽타주 실행
	AnimInstance->PlayAnimMontage(FireAttackMontage);

	// OnMeleeAttackMontageEndedDelegate 바인드 된 것이 없다면 바인드 설정
	if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, FireAttackMontage);
	}

}

void ASNonViewCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	// 걷기 가능
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// 공격 중이 아닌 것으로 변환
	bIsNowAttacking = false;

	// 바인드 되어 있다면 해제 작업
	if (true == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}
