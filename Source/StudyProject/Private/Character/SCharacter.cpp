// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Animation/SAnimInstance.h"
#include "Item/SWeaponActor.h"
#include "Game/SGameState.h"
#include "Component/SStatComponent.h"
#include "Controller/SPlayerController.h"

int32 ASCharacter::ShowAttackDebug = 0;
FAutoConsoleVariableRef CVarShowAttackDebug(TEXT("StudyProject.ShowAttackDebug"), ASCharacter::ShowAttackDebug, TEXT(""), ECVF_Cheat);

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    float CharacterHalfHeight = 95.0f;
    float CharacterRadius = 20.0f;

    GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter"));

    FVector PivotPosition(0.0f, 0.0f, -CharacterHalfHeight);
    FRotator PivotRotation(0.0f, -90.0f, 0.0f);
    GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 400.0f;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    // ���� �� �ִ� �ִ� �ӵ�
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    // �Ƴ��α� �Է��� �ּ� ��
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    // ������
    GetCharacterMovement()->JumpZVelocity = 700.0f;
    // ���߿� ���� �� ������ ���� ����
    GetCharacterMovement()->AirControl = 0.35f;
    // �ȴٰ� ������ �� �����Ÿ�, ���� Ŭ���� �� ������ ����
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

    //bIsDead = false;
    StatComponent = CreateDefaultSubobject<USStatComponent>(TEXT("StatComponent"));
    StatComponent->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    // ��������Ʈ ���� ����
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == IsValid(AnimInstance))
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnMeleeAttackMontageEnded);
        //AnimInstance->OnCheckHit.AddDynamic(this, &ThisClass::OnCheckHit);
        AnimInstance->OnCheckAttackInput.AddDynamic(this, &ThisClass::OnCheckAttackInput);
    }

    if (true == IsValid(StatComponent) && false == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    {
        StatComponent->OnOutOfCurrentHPDelegate.AddDynamic(this, &ThisClass::OnCharacterDeath);
    }

}

float ASCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // Waiting ���¿����� Damage�� ���� �ʰ� �ϱ� ����
    ASGameState* SGameState = Cast<ASGameState>(UGameplayStatics::GetGameState(this));
    if (true == IsValid(SGameState) && EMatchState::Playing != SGameState->MatchState)
    {
        return FinalDamageAmount;
    }

    //CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.0f, MaxHP);

    //// KINDA_SMALL_NUMBER: 0�� ����� �ٻ�ġ
    //if (KINDA_SMALL_NUMBER > CurrentHP)
    //{
    //    bIsDead = true;
    //    CurrentHP = 0.0f;
    //    // �׾��� �� �ݸ��� �� �����Ʈ�� ���� ���� �ʰ� ����
    //    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    //}

    StatComponent->SetCurrentHP(StatComponent->GetCurrentHP() - FinalDamageAmount);

    if (1 == ShowAttackDebug)
    {
        //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s was taken damage: %.3f"), *GetName(), FinalDamageAmount));
        //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), CurrentHP, MaxHP));
        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), StatComponent->GetCurrentHP(), StatComponent->GetMaxHP()));
    }

    return FinalDamageAmount;
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, WeaponInstance);

}

void ASCharacter::OnMeleeAttackMontageEnded(UAnimMontage* InMontage, bool bInterruped)
{
    // Ư�� AM_Rifle_Fire_Melee�� �ִϸ��̼��� ������ ����
    if (true == InMontage->GetName().Equals(TEXT("AM_Rifle_Fire_Melee"), ESearchCase::IgnoreCase))
    {
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        bIsNowAttacking = false;
    }

}

void ASCharacter::OnCheckHit()
{
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CheckHit() has been called.")));

    FHitResult HitResult;
    // �Ű�����: �±׸�, ������ �ݸ��� ���� ����� ������(0, 1), ���õ� ����(this�� �� ������ �ڽ��� ���ݿ� �ڽ��� ������ �ʱ� ����)
    FCollisionQueryParams Params(NAME_None, false, this);

    // Sweep: ó�����, Single: ����, ByChannel: ��� �������� �˻�?
    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,                                                          // �浹�� ������ ���� ����ü ����
        GetActorLocation(),                                                 // �浹 Ž��������
        GetActorLocation() + MeleeAttackRange * GetActorForwardVector(),    // �浹 Ž�� ������
        FQuat::Identity,                                                    // Ž�� ������ ȸ��
        ECC_GameTraceChannel2,                                              // �浹 ���� ä��
        FCollisionShape::MakeSphere(MeleeAttackRadius),                     // Ư�� �������� ������� �浹 ����
        Params                                                              // Ž�� ����� ���� ���� ����ü ����
    );

    // �浹�� �� ���
    if (true == bResult)
    {
        if (true == IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            // TakeDamage�Ű�����: ����� ũ��, ����� ����, ���� ����� ���� ��Ʈ�ѷ�, ����� �ִ� ����
            HitResult.GetActor()->TakeDamage(50.0f, DamageEvent, GetController(), this);
        }
    }
#pragma region CollisionDebugDrawing
    if (1 == ShowAttackDebug)
    {
        FVector TraceVector = MeleeAttackRange * GetActorForwardVector();
        FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.0f;
        float HalfHeight = MeleeAttackRange * 0.5f + MeleeAttackRadius;
        FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
        FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
        float DebugLifeTime = 5.0f;

        DrawDebugCapsule(
            GetWorld(),
            Center,                 // ���� ����
            HalfHeight,             // ĸ���� ���� ����
            MeleeAttackRadius,      // ������
            CapsuleRot,             // ��������� ȸ��?
            DrawColor,              // �浹X: ����, �浹O: �ʷ� (�� �������� ������ ��)
            false,
            DebugLifeTime           // ���ӽð�
        );

        if (true == bResult)
        {
            if (true == IsValid(HitResult.GetActor()))
            {
                UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
            }
        }
    }
#pragma endregion

}

void ASCharacter::OnCheckAttackInput()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    checkf(true == IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
    checkf(true == IsValid(WeaponInstance), TEXT("Invalid WeaponInstance"));

    if (true == bIsAttackKeyPressed)
    {
        // �޺� ī��Ʈ ���
        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

        // �޺� ���� �̵�
        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
        AnimInstance->Montage_JumpToSection(NextSectionName, WeaponInstance->GetMeleeAttackMontage());
        bIsAttackKeyPressed = false;
    }
}

void ASCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
    checkf(true == IsValid(WeaponInstance), TEXT("Invalid WeaponInstance"));
    ensureMsgf(0 != CurrentComboCount, TEXT("0 == CurrentComboCount"));

    // �ʱ�ȭ �۾�
    CurrentComboCount = 0;
    bIsAttackKeyPressed = false;
    bIsNowAttacking = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    // �����ڵ�
    if (true == OnMeleeAttackMontageEndedDelegate.IsBound()) OnMeleeAttackMontageEndedDelegate.Unbind();
}

void ASCharacter::OnCharacterDeath()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    // ��Ʈ�ѷ��� Ȯ���ؼ� ĳ���Ͱ� �׾����� �˸�
    // ĳ���� -> ��Ʈ�ѷ� -> ���Ӹ��
    ASPlayerController* PlayerController = GetController<ASPlayerController>();
    if (true == IsValid(PlayerController) && true == HasAuthority())
    {
        PlayerController->OnOwningCharacterDead();
    }
}

void ASCharacter::BeginAttack()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    checkf(true == IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
    checkf(true == IsValid(WeaponInstance), TEXT("Invalid WeaponInstance"));

    // ���߱�
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    // ���� ������ Ȯ��
    bIsNowAttacking = true;
    // ��Ÿ�� �÷��� ����
    AnimInstance->PlayAnimMontage(WeaponInstance->GetMeleeAttackMontage());

    CurrentComboCount = 1;

    if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
    {
        // Ư���� ��Ÿ�ְ� ������ ���� ������ �۾��� �ϱ� ����
        // ���� Montage_SetEndDelegate�� ����Ͽ� Ư�� ��Ÿ��(OnMeleeAttackMontageEndedDelegate)�� ������ ���� ȣ��ǰ�!
        // ȣ�� �۾� ���� �̸� ��������Ʈ�� Ư�� ȣ��� �Լ��� ���ε��Ͽ�
        // Ư�� WeaponInstance->GetMeleeAttackMontage()�� ������ ���� OnMeleeAttackMontageEndedDelegate ��ε�ĳ��Ʈ�� ȣ��Ǵ� ���(EndAttack�� ȣ��)
        OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
        AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, WeaponInstance->GetMeleeAttackMontage());
    }
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// BeginPlay���� ��������Ʈ ���ε� �ɾ���� �۾��� EndPlay���� ���� �۾� ����� ��
void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (true == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    {
        StatComponent->OnOutOfCurrentHPDelegate.RemoveDynamic(this, &ThisClass::OnCharacterDeath);
    }

    Super::EndPlay(EndPlayReason);
}

