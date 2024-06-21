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
	// ASNonViewCharacter�� ������ ��ġ�ǰų� ���Ӱ� �����Ǹ� SAIController�� ���ǰ� �ڵ����� ����
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// ���� ������Ʈ ���� �� RootComponent�� ����
	WidgetComponent = CreateDefaultSubobject<USWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	// ��ġ�� ĳ���� �Ӹ����� ���̱� ���� Z�����κ��� 100.0f���� ����
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	// EWidgetSpace::Screen�� Billboard ���(ī�޶� ���� UI�� ȸ��)���� ���̳� �÷��̾� ĳ���͸� ������ ��. ���� UI�� �־����� ������ ũ�Ⱑ ����
	// WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// EWidgetSpace::World�� �ϸ� UI�� �־����� ũ�Ⱑ �۾���, UI ȸ���� �ȵ�
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

	// Super::TakeDamage()���� ������ ó��
	//if (KINDA_SMALL_NUMBER > CurrentHP)
	if (KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
	{
		ASAIController* AIController = Cast<ASAIController>(GetController());
		if (true == IsValid(AIController))
		{
			AIController->EndAI();
		}

		// ���� ���Ͱ� ASViewCharacter��� ų ī��Ʈ ����
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
		// �����带 �ƿ� �� �� ���� ���� ���� ���� �� �÷��̾ �ٶ󺸰� �������� ��

		FVector WidgetComponentLocation = WidgetComponent->GetComponentLocation();
		// UGameplayStatics::GetPlayerCameraManager(this, 0)�� ���� ���� �����ϴ� �÷��̾ ����
		FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
		// WidgetComponentLocation���� LocalPlayerCameraLocation�� �ٶ󺸴� ������ ȸ������ ����
		// �� ������ �÷��̾ ���� �ٶ󺸰� ��
		WidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));
	}

}

void ASNonViewCharacter::SetWidget(UStudyWidget* InStudyWidget)
{
	Super::SetWidget(InStudyWidget);

	USW_HPBar* HPBarWidget = Cast<USW_HPBar>(InStudyWidget);
	if (true == IsValid(HPBarWidget))
	{
		// ���� �ʱ�ȭ
		HPBarWidget->SetMaxHP(StatComponent->GetMaxHP());
		HPBarWidget->InitializeHPBarWidget(StatComponent);
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBarWidget, &USW_HPBar::OnCurrentHPChange);
	}

}

void ASNonViewCharacter::BeginAttack()
{
	// �ִ� �ν��Ͻ� ��������
	USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	// ������ ����
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// ���� ������ ��ȯ
	bIsNowAttacking = true;
	// ��Ÿ�� ����
	AnimInstance->PlayAnimMontage(FireAttackMontage);

	// OnMeleeAttackMontageEndedDelegate ���ε� �� ���� ���ٸ� ���ε� ����
	if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, FireAttackMontage);
	}

}

void ASNonViewCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	// �ȱ� ����
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// ���� ���� �ƴ� ������ ��ȯ
	bIsNowAttacking = false;

	// ���ε� �Ǿ� �ִٸ� ���� �۾�
	if (true == OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}
