// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SViewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Particles/ParticleSystemComponent.h"
#include "Input/SInputConfigData.h"
#include "Item/SWeaponActor.h"
#include "Animation/SAnimInstance.h"
#include "Component/SStatComponent.h"

//int32 ASViewCharacter::ShowAttackDebug = 0;
//FAutoConsoleVariableRef CVarShowAttackDebug(TEXT("StudyProject.ShowAttackDebug"), ASViewCharacter::ShowAttackDebug, TEXT(""), ECVF_Cheat);

ASViewCharacter::ASViewCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    // AutoActivate: 생성되면 바로 켜지는데 원할 때 키고 끌 수 있게끔 현재는 false로 세팅
    ParticleSystemComponent->SetAutoActivate(false);

}

void ASViewCharacter::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (true == IsValid(PlayerController))
    {
        // subsystem을 통해 싱글톤 개체를 가져온다
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (true == IsValid(Subsystem))
        {
            Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
        }
    }

}

void ASViewCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 뷰모드 설정
    //SetViewMode(EViewMode::BackView);
    //DestArmLength = 400.0f; // 초기화에서 한 번 지정해야 함
    //DestArmRotation = FRotator::ZeroRotator; // 백뷰에서는 의미가 없으므로
    SetViewMode(EViewMode::TPSView);
}

void ASViewCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (CurrentViewMode)
    {
    case EViewMode::BackView:
        break;
    case EViewMode::QuarterView:
    {
        // DirectionToMove값이 변했다?
        // SizeSquared은 절대값으로 어떠한 길이를 나타냄
        if (KINDA_SMALL_NUMBER < DirectionToMove.SizeSquared())
        {
            // X축 벡터 만들기
            GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
            // 해당 방향으로 이동
            AddMovementInput(DirectionToMove);
            // 0으로 초기화
            DirectionToMove = FVector::ZeroVector;
        }
        break;
    }
    case EViewMode::End:
        break;
    default:
        break;
    }

    if (KINDA_SMALL_NUMBER < abs(DestArmLength - SpringArmComponent->TargetArmLength))
    {
        // 보간작업
        SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, DestArmLength, DeltaTime, ArmLengthChangeSpeed);
        SpringArmComponent->SetRelativeRotation(FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), DestArmRotation, DeltaTime, ArmRotationChangeSpeed));
    }
}

void ASViewCharacter::SetViewMode(EViewMode InViewMode)
{
    // 같은 뷰모드이면 무시
    if (CurrentViewMode == InViewMode)
    {
        return;
    }

    // 뷰모드 업데이트
    CurrentViewMode = InViewMode;

    switch (CurrentViewMode)
    {
    case EViewMode::BackView:
        //SpringArmComponent->TargetArmLength = 400.0f;
        //// ControlRotation이 Pawn의 회전과 동기화 -> Pawn의 회전이 SprintArm의 회전 동기화. 이로 인해 SetRotation()이 무의미
        //SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        SpringArmComponent->bUsePawnControlRotation = true;
        SpringArmComponent->bDoCollisionTest = true;

        SpringArmComponent->bInheritPitch = true;
        SpringArmComponent->bInheritYaw = true;
        SpringArmComponent->bInheritRoll = false;

        // 회전방향에 따른 메쉬 회전
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
        GetCharacterMovement()->bOrientRotationToMovement = true;   // 가고자 하는 방향으로 회전되는걸 허용하겠다
        GetCharacterMovement()->bUseControllerDesiredRotation = false;

        break;
    case EViewMode::QuarterView:
        /*SpringArmComponent->TargetArmLength = 900.0f;
        SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));*/

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        SpringArmComponent->bUsePawnControlRotation = false;
        SpringArmComponent->bDoCollisionTest = false;

        SpringArmComponent->bInheritPitch = false;
        SpringArmComponent->bInheritYaw = false;
        SpringArmComponent->bInheritRoll = false;

        // 45도씩 끊겨서 움직이는 것을 부드럽게 바꾸기 위함
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = true;

        break;
    case EViewMode::TPSView:
    {
        SpringArmComponent->TargetArmLength = 400.0f;
        SpringArmComponent->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;

        SpringArmComponent->bUsePawnControlRotation = true;
        SpringArmComponent->bDoCollisionTest = true;

        SpringArmComponent->bInheritPitch = true;
        SpringArmComponent->bInheritYaw = true;
        SpringArmComponent->bInheritRoll = false;

        GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;

        break;
    }
    case EViewMode::End:
        break;
    default:
        break;
    }
}

//void ASViewCharacter::AddCurrentKillCount(int32 InCurrentKillCount)
//{
//    CurrentKillCount = FMath::Clamp(CurrentKillCount + InCurrentKillCount, 0.0f, MaxKillCount);
//    ParticleSystemComponent->Activate(true);
//
//}

void ASViewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Move, ETriggerEvent::Triggered, this, &ASViewCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Look, ETriggerEvent::Triggered, this, &ASViewCharacter::Look);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ChangeView, ETriggerEvent::Started, this, &ASViewCharacter::ChangeView);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->QuickSlot01, ETriggerEvent::Started, this, &ThisClass::InputQuickSlot01);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->QuickSlot02, ETriggerEvent::Started, this, &ThisClass::InputQuickSlot02);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Attack, ETriggerEvent::Started, this, &ThisClass::Attack);
    }
}

void ASViewCharacter::Move(const FInputActionValue& InValue)
{
    //if (MOVE_None == GetCharacterMovement()->GetGroundMovementMode() || true == bIsDead) return;
    if (MOVE_None == GetCharacterMovement()->GetGroundMovementMode() || KINDA_SMALL_NUMBER >= StatComponent->GetCurrentHP()) return;

    FVector2D MovementVector = InValue.Get<FVector2D>();

    /*AddMovementInput(GetActorForwardVector(), MovementVector.X);
    AddMovementInput(GetActorRightVector(), MovementVector.Y);*/

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::TPSView:
    case EViewMode::BackView:
    // Switch-Case 구문 내에서 Scope를 지정하면 해당 Scope 내에서 변수 선언이 가능
    { 
        // 카메라가 바라보는 방향으로 회전하기 위한 코드

        const FRotator ControlRotation = GetController()->GetControlRotation();
        const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.0f);

        // 해당 축 행렬의 단위 벡터를 이용해 방향 변경
        const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
        const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardVector, MovementVector.X);
        AddMovementInput(RightVector, MovementVector.Y);

        ForwardInputValue = MovementVector.X;
        RightInputValue = MovementVector.Y;

        break;
    }
    case EViewMode::QuarterView:
        DirectionToMove.X = MovementVector.X;
        DirectionToMove.Y = MovementVector.Y;
        break;
    case EViewMode::End:
        break;
    default:
        AddMovementInput(GetActorForwardVector(), MovementVector.X);
        AddMovementInput(GetActorRightVector(), MovementVector.Y);
        break;
    }
}

void ASViewCharacter::Look(const FInputActionValue& InValue)
{
    //if (true == IsValid(GetController()))
    //{
    //    FVector2D LookVector = InValue.Get<FVector2D>();

    //    // 좌우
    //    AddControllerYawInput(LookVector.X);
    //    // 상하
    //    AddControllerPitchInput(LookVector.Y);
    //}

    //if (MOVE_None == GetCharacterMovement()->GetGroundMovementMode() || true == bIsDead) return;
    if (MOVE_None == GetCharacterMovement()->GetGroundMovementMode() || KINDA_SMALL_NUMBER >= StatComponent->GetCurrentHP()) return;

    FVector2D LookVector = InValue.Get<FVector2D>();

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::TPSView:
    case EViewMode::BackView:
        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(LookVector.Y);
        break;
    case EViewMode::QuarterView:
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }
}

void ASViewCharacter::ChangeView(const FInputActionValue& InValue)
{
    switch (CurrentViewMode)
    {
    case EViewMode::BackView:
        /* Case 1. 이전에 BackView 시점이었다면

          BackView는 컨트롤 회전값 == 스프링암 회전값.
          그러나 QuarterView는 캐릭터의 회전값 == 컨트롤 회전값.
          따라서 시점 변경 전에 캐릭터의 현재 회전값을 컨트롤 회전에 세팅해둬야 함.
          안그러면 컨트롤 회전이 일어나면서 현재 캐릭터의 회전값이 스프링암 회전값(컨트롤 회전값)으로 동기화됨.
        */
        GetController()->SetControlRotation(GetActorRotation());
        DestArmLength = 900.0f;
        DestArmRotation = FRotator(-45.0f, 0.0f, 0.0f);
        SetViewMode(EViewMode::QuarterView);
        break;
    case EViewMode::QuarterView:
        /* Case 2. 이전에 QuarterView 시점이었다면

          컨트롤 회전이 캐릭터 회전에 맞춰져 있을거임.
          //QuarterView는 현재 스프링암의 회전값을 컨트롤 회전에 세팅해둔 상태에서 시점 변경해야 올바름.
          BackView에서는 컨트롤 로테이션이 폰의 회전과 동기화되고 폰의 회전이 스프링 암의 회전과 동기화.
          따라서 스프링 암의 회전을 임의로 설정할 수 없음. 0으로 고정.
        */
        GetController()->SetControlRotation(FRotator::ZeroRotator);
        DestArmLength = 400.0f;
        DestArmRotation = FRotator::ZeroRotator;
        SetViewMode(EViewMode::BackView);
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }
}

// 장착 했을 때 소켓을 찾아서 설정
void ASViewCharacter::InputQuickSlot01(const FInputActionValue& InValue)
{
    FName WeaponSocket(TEXT("WeaponSocket"));
    if (true == GetMesh()->DoesSocketExist(WeaponSocket) && false == IsValid(WeaponInstance))
    {
        WeaponInstance = GetWorld()->SpawnActor<ASWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (true == IsValid(WeaponInstance))
        {
            WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
        }
    }
}

// 해제 했을 때 처리
void ASViewCharacter::InputQuickSlot02(const FInputActionValue& InValue)
{
    if (true == IsValid(WeaponInstance))
    {
        WeaponInstance->Destroy();
        WeaponInstance = nullptr;
    }
}

void ASViewCharacter::Attack(const FInputActionValue& InValue)
{
    /*if (true == IsValid(WeaponInstance))
    {
        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Attack() has been called.")));
    }*/

    // 점프 중 공격X
    if (true == GetCharacterMovement()->IsFalling()) return;

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    checkf(true == IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

    if (true == IsValid(WeaponInstance))
    {
        if (true == IsValid(WeaponInstance->GetMeleeAttackMontage()))
        {
            /*AnimInstance->PlayAnimMontage(WeaponInstance->GetMeleeAttackMontage());

            GetCharacterMovement()->SetMovementMode(MOVE_None);
            bIsNowAttacking = true;*/

            // 첫 콤보
            if (0 == CurrentComboCount) BeginAttack();
            // 콤보공격
            else
            {
                ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
                bIsAttackKeyPressed = true;
            }
        }
    }
}
