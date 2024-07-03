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

    // 걸을 수 있는 최대 속도
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    // 아날로그 입력의 최소 값
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    // 점프력
    GetCharacterMovement()->JumpZVelocity = 700.0f;
    // 공중에 떴을 때 앞으로 가는 정도
    GetCharacterMovement()->AirControl = 0.35f;
    // 걷다가 멈췄을 때 제동거리, 값이 클수록 더 빠르게 멈춤
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

    //bIsDead = false;
    StatComponent = CreateDefaultSubobject<USStatComponent>(TEXT("StatComponent"));
    StatComponent->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    // 델리게이트 연결 설정
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
    
    // Waiting 상태에서는 Damage가 들어가지 않게 하기 위함
    ASGameState* SGameState = Cast<ASGameState>(UGameplayStatics::GetGameState(this));
    if (true == IsValid(SGameState) && EMatchState::Playing != SGameState->MatchState)
    {
        return FinalDamageAmount;
    }

    //CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.0f, MaxHP);

    //// KINDA_SMALL_NUMBER: 0에 가까운 근사치
    //if (KINDA_SMALL_NUMBER > CurrentHP)
    //{
    //    bIsDead = true;
    //    CurrentHP = 0.0f;
    //    // 죽었을 떄 콜리전 및 무브먼트가 영향 받지 않게 세팅
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
    // 특정 AM_Rifle_Fire_Melee의 애니메이션이 끝났을 때만
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
    // 매개변수: 태그명, 복잡한 콜리전 구조 사용할 것인지(0, 1), 무시될 액터(this를 한 이유는 자신의 공격에 자신이 당하지 않기 위함)
    FCollisionQueryParams Params(NAME_None, false, this);

    // Sweep: 처리대상, Single: 개수, ByChannel: 어떤걸 기준으로 검사?
    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,                                                          // 충돌된 정보가 담기는 구조체 변수
        GetActorLocation(),                                                 // 충돌 탐지시작점
        GetActorLocation() + MeleeAttackRange * GetActorForwardVector(),    // 충돌 탐지 끝지점
        FQuat::Identity,                                                    // 탐색 도형의 회전
        ECC_GameTraceChannel2,                                              // 충돌 감지 채널
        FCollisionShape::MakeSphere(MeleeAttackRadius),                     // 특정 사이즈의 모양으로 충돌 감지
        Params                                                              // 탐색 방법에 대한 설정 구조체 변수
    );

    // 충돌이 된 경우
    if (true == bResult)
    {
        if (true == IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            // TakeDamage매개변수: 대미지 크기, 대미지 종류, 공격 명령을 내린 컨트롤러, 대미지 주는 액터
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
            Center,                 // 시작 지점
            HalfHeight,             // 캡슐의 절반 높이
            MeleeAttackRadius,      // 반지름
            CapsuleRot,             // 어느쪽으로 회전?
            DrawColor,              // 충돌X: 빨강, 충돌O: 초록 (위 변수에서 설정한 것)
            false,
            DebugLifeTime           // 지속시간
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
        // 콤보 카운트 계산
        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

        // 콤보 섹션 이동
        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
        AnimInstance->Montage_JumpToSection(NextSectionName, WeaponInstance->GetMeleeAttackMontage());
        bIsAttackKeyPressed = false;
    }
}

void ASCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
    checkf(true == IsValid(WeaponInstance), TEXT("Invalid WeaponInstance"));
    ensureMsgf(0 != CurrentComboCount, TEXT("0 == CurrentComboCount"));

    // 초기화 작업
    CurrentComboCount = 0;
    bIsAttackKeyPressed = false;
    bIsNowAttacking = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    // 정리코드
    if (true == OnMeleeAttackMontageEndedDelegate.IsBound()) OnMeleeAttackMontageEndedDelegate.Unbind();
}

void ASCharacter::OnCharacterDeath()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    // 컨트롤러를 확인해서 캐릭터가 죽었음을 알림
    // 캐릭터 -> 컨트롤러 -> 게임모드
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

    // 멈추기
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    // 공격 중인지 확인
    bIsNowAttacking = true;
    // 몽타주 플레이 시작
    AnimInstance->PlayAnimMontage(WeaponInstance->GetMeleeAttackMontage());

    CurrentComboCount = 1;

    if (false == OnMeleeAttackMontageEndedDelegate.IsBound())
    {
        // 특정한 몽타주가 끝났을 때만 별도의 작업을 하기 원함
        // 따라서 Montage_SetEndDelegate을 사용하여 특정 몽타주(OnMeleeAttackMontageEndedDelegate)가 끝났을 때만 호출되게!
        // 호출 작업 전에 미리 델리게이트로 특정 호출될 함수를 바인드하여
        // 특정 WeaponInstance->GetMeleeAttackMontage()가 끝났을 때만 OnMeleeAttackMontageEndedDelegate 브로드캐스트가 호출되는 방식(EndAttack가 호출)
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

// BeginPlay에서 델리게이트 바인드 걸어줬던 작업을 EndPlay에서 해제 작업 해줘야 함
void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (true == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    {
        StatComponent->OnOutOfCurrentHPDelegate.RemoveDynamic(this, &ThisClass::OnCharacterDeath);
    }

    Super::EndPlay(EndPlayReason);
}

