// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SViewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Input/SInputConfigData.h"
#include "Item/SWeaponActor.h"
#include "Animation/SAnimInstance.h"
#include "Component/SStatComponent.h"
#include "Controller/SPlayerController.h"
#include "Game/SPlayerState.h"
#include "WorldStatic/SLandMine.h"
#include "SViewCharacterSettings.h"             // 모듈에 추가해줬기 때문에 가능

//int32 ASViewCharacter::ShowAttackDebug = 0;
//FAutoConsoleVariableRef CVarShowAttackDebug(TEXT("StudyProject.ShowAttackDebug"), ASViewCharacter::ShowAttackDebug, TEXT(""), ECVF_Cheat);

ASViewCharacter::ASViewCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    // AutoActivate: 생성되면 바로 켜지는데 원할 때 키고 끌 수 있게끔 현재는 false로 세팅
    ParticleSystemComponent->SetAutoActivate(false);

    const USViewCharacterSettings* CDO = GetDefault<USViewCharacterSettings>();
    if (0 < CDO->PlayerCharacterMeshMaterialPaths.Num())
    {
        for (FSoftObjectPath PlayerCharacterMeshPath : CDO->PlayerCharacterMeshMaterialPaths)
        {
            UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *(PlayerCharacterMeshPath.ToString()));
        }
    }

    TimeBetweenFire = 60.0f / FirePerMinute;

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

    //// CDO 가져오기
    //const USViewCharacterSettings* CDO = GetDefault<USViewCharacterSettings>();
    //// 0 ~ 보유한 개수만큼 중 하나를 랜덤으로 고르기
    //int32 RandIndex = FMath::RandRange(0, CDO->PlayerCharacterMeshMaterialPaths.Num() - 1);
    //CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[RandIndex];
    //// RequestAsyncLoad: 비동기 요청
    //// CurrentPlayerCharacterMeshMaterialPath 경로랑 FStreamableDelegate::CreateLambda 델리게이트 람다를 사용하여 비동기 요청
    //AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
    //    CurrentPlayerCharacterMeshMaterialPath,
    //    FStreamableDelegate::CreateLambda([this]() -> void
    //        {
    //            // 핸들 정리
    //            AssetStreamableHandle->ReleaseHandle();
    //            // 마테리얼 얻기
    //            TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
    //            // 얻는데 성공하여 유효한 값이면
    //            if (true == LoadedMaterialInstanceAsset.IsValid())
    //            {
    //                // 해당 마테리얼로 세팅
    //                // 0번인 이유는 바디를 설정하기 위함
    //                GetMesh()->SetMaterial(0, LoadedMaterialInstanceAsset.Get());
    //            }
    //        })
    //);

    // 클라에서도 적용되게 BeginPlay에서 세팅
    SetViewMode(EViewMode::TPSView);

}

// 서버에서만 돌아가는 로직
void ASViewCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 뷰모드 설정
    //SetViewMode(EViewMode::BackView);
    //DestArmLength = 400.0f; // 초기화에서 한 번 지정해야 함
    //DestArmRotation = FRotator::ZeroRotator; // 백뷰에서는 의미가 없으므로
    //SetViewMode(EViewMode::TPSView);         // 따라서 클라에서는 돌아가지 않게 됨
}

void ASViewCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 줌 전환 자연스러움을 위해 Tick에서 적용
    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 35.0f);
    CameraComponent->SetFieldOfView(CurrentFOV);

    // 컨트롤러가 유효하다면
    if (true == IsValid(GetController()))
    {
        // 해당 컨트롤러 정보 관리 => 애니메이션 회전 방향을 위한 작업
        /*FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;*/

        PreviousAimPitch = CurrentAimPitch;
        PreviousAimYaw = CurrentAimYaw;

        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;

    }

    // Pitch 혹은 Yaw 값이 달라졌을 때 서버에서 처리하여 클라에 뿌리는 작업
    if (PreviousAimPitch != CurrentAimPitch || PreviousAimYaw != CurrentAimYaw)
    {
        if (false == HasAuthority())
        {
            UpdateAimValue_Server(CurrentAimPitch, CurrentAimYaw);

        }

    }

    // 앞뒤 혹은 좌우 값이 달라졌을 때 서버에서 처리하여 클라에 뿌리는 작업
    if (PreviousForwardInputValue != ForwardInputValue || PreviousRightInputValue != RightInputValue)
    {
        if (false == HasAuthority())
        {
            UpdateInputValue_Server(ForwardInputValue, RightInputValue);

        }

    }

    // 블렌딩 작업 bool 값이 켜져 있다면
    if (true == bIsNowRagdollBlending)
    {
        CurrentRagDollBlendWeight = FMath::FInterpTo(CurrentRagDollBlendWeight, TargetRagDollBlendWeight, DeltaTime, 10.0f);

        FName PivotBoneName = FName(TEXT("spine_01"));
        // 본에 관해 어느 정도 랙돌을 작동 시킬 것인가
        GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagDollBlendWeight);

        // 블렌드 수치 적용 연산을 통해 0에 가까운 수치(보간 끝)가 되면 랙돌 끄기 
        if (KINDA_SMALL_NUMBER > CurrentRagDollBlendWeight - TargetRagDollBlendWeight)
        {
            GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
            bIsNowRagdollBlending = false;
        }

        // HP가 0에 가깝다면
        if (true == IsValid(GetStatComponent()) && KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
        {
            // 모든 본에 랙돌 가중치(기존에 상체만 주던 것을 전체인 root로 잡아줌)
            GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("root")), 1.0f);
            GetMesh()->SetSimulatePhysics(true);
            // 다음 Tick에서 걸리지 않게 false
            bIsNowRagdollBlending = false;
        }

    }

    return;

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

float ASViewCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (false == IsValid(GetStatComponent())) return FinalDamage;

    // 현재 HP가 0에 가까운 상태라면
    if (KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
    {
        // 완전 랙돌 상태 On
        GetMesh()->SetSimulatePhysics(true);
    }
    // 맞았지만 아직 HP가 남아있다면
    else
    {
        // 상체를 기준으로 하는 본 이름
        FName PivotBoneName = FName(TEXT("spine_01"));
        // 본 네임을 통해 해당 본은 시뮬레이션 피직스 키기
        GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);

        // 랙돌 포즈에 완전 치우쳐지게끔 가중치를 1.0f로 지정
        // 즉 해당 부분은 랙돌 상태
        //float BlendWeight = 1.0f; 
        //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

        // 1: 랙돌 키기, 0: 랙돌 끄기
        TargetRagDollBlendWeight = 1.0f;

        // 타이머 관련 델리게이트 바인드 작업
        HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
        // 타이머 시간이 되면 작동해야 되는 함수 호출
        GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.0f, false);
    }

    return FinalDamage;
}

void ASViewCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, ForwardInputValue);
    DOREPLIFETIME(ThisClass, RightInputValue);
    DOREPLIFETIME(ThisClass, CurrentAimPitch);
    DOREPLIFETIME(ThisClass, CurrentAimYaw);
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

void ASViewCharacter::SetMeshMaterial(const EPlayerTeam& InPlayerTeam)
{
    uint8 TeamIdx = 0u;
    switch (InPlayerTeam)
    {
    case EPlayerTeam::Black:
        TeamIdx = 0u;
        break;
    case EPlayerTeam::White:
        TeamIdx = 1u;
        break;
    default:
        break;
    }

    // CDO 가져오기
    const USViewCharacterSettings* CDO = GetDefault<USViewCharacterSettings>();
    CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[TeamIdx];
    // RequestAsyncLoad: 비동기 요청
    // CurrentPlayerCharacterMeshMaterialPath 경로랑 FStreamableDelegate::CreateLambda 델리게이트 람다를 사용하여 비동기 요청
    AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        CurrentPlayerCharacterMeshMaterialPath,
        FStreamableDelegate::CreateLambda([this]() -> void
            {
                // 핸들 정리
                AssetStreamableHandle->ReleaseHandle();
                // 마테리얼 얻기
                TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
                // 얻는데 성공하여 유효한 값이면
                if (true == LoadedMaterialInstanceAsset.IsValid())
                {
                    // 해당 마테리얼로 세팅
                    // 0번인 이유는 바디를 설정하기 위함
                    GetMesh()->SetMaterial(0, LoadedMaterialInstanceAsset.Get());
                }
            })
    );
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
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Menu, ETriggerEvent::Started, this, &ThisClass::Menu);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSight, ETriggerEvent::Started, this, &ThisClass::StartIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSight, ETriggerEvent::Completed, this, &ThisClass::EndIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Trigger, ETriggerEvent::Started, this, &ThisClass::ToggleTrigger);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Attack, ETriggerEvent::Started, this, &ThisClass::StartFire);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Attack, ETriggerEvent::Completed, this, &ThisClass::StopFire);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LandMine, ETriggerEvent::Started, this, &ThisClass::SpawnLandMine);
    }
}

// WithValidation 사용시
// Validate: 변위조 검증 조건 담당
bool ASViewCharacter::SpawnLandMine_Server_Validate()
{
    return true;
}

// Implementation: 로직 구현 담당
void ASViewCharacter::SpawnLandMine_Server_Implementation()
{
    if (true == IsValid(LandMineClass))
    {
        FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 200.0f) - FVector(0.0f, 0.0f, 90.0f);
        ASLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
        SpawnedLandMine->SetOwner(GetController());
    }

}

// 클라가 아닌 서버에서 이루어짐
void ASViewCharacter::SpawnWeaponInstance_Server_Implementation()
{
    FName WeaponSocket(TEXT("WeaponSocket"));
    if (true == GetMesh()->DoesSocketExist(WeaponSocket) && false == IsValid(WeaponInstance))
    {
        // 스폰시 WeaponInstance 값 변경
        // 해당 값이 변경될 때 OnRep_WeaponInstance() 호출되며 Replicated
        WeaponInstance = GetWorld()->SpawnActor<ASWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (true == IsValid(WeaponInstance))
        {
            WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
        }
    }

}

// 클라가 아닌 서버에서 이루어짐
void ASViewCharacter::DestroyWeaponInstance_Server_Implementation()
{
    if (true == IsValid(WeaponInstance))
    {
        WeaponInstance->Destroy();
        WeaponInstance = nullptr;
    }
}

void ASViewCharacter::OnRep_WeaponInstance()
{
    // 스폰이 되었고 복제가 된 상태에서 여기까지 들어왔다
    if (true == IsValid(WeaponInstance))
    {
        // 라이플 애님 레이어로 연결
        TSubclassOf<UAnimInstance> RifleCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
        if (true == IsValid(RifleCharacterAnimLayer))
        {
            GetMesh()->LinkAnimClassLayers(RifleCharacterAnimLayer);
        }

        // 장착 몽타주 실행
        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance->GetEquipAnimMontage()))
        {
            AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
        }

        // 레이어 및 몽타주 저장
        UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
        UnequipAnimMontage = WeaponInstance->GetUnequipAnimMontage();

    }
    // 장착 해제 과정을 거치면서 DestroyWeaponInstance_Server를 지나 여기로 오면
    // 이전에 저장해 놓은 레이어 및 몽타주를 실행
    else
    {
        if (true == IsValid(UnarmedCharacterAnimLayer))
        {
            GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
        }

        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == IsValid(UnequipAnimMontage))
        {
            AnimInstance->Montage_Play(UnequipAnimMontage);
        }

    }

}

void ASViewCharacter::UpdateInputValue_Server_Implementation(const float& InForwardInputValue, const float& InRightInputValue)
{
    ForwardInputValue = InForwardInputValue;
    RightInputValue = InRightInputValue;

}

void ASViewCharacter::UpdateAimValue_Server_Implementation(const float& InAimPitchValue, const float& InAimYawValue)
{
    CurrentAimPitch = InAimPitchValue;
    CurrentAimYaw = InAimYawValue;

}

void ASViewCharacter::OnHittedRagdollRestoreTimerElapsed()
{
    // 상체를 기준으로 하는 본 이름
    FName PivotBoneName = FName(TEXT("spine_01"));
    // 본 네임을 통해 해당 본은 시뮬레이션 피직스 끄기
    //GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);

    //float BlendWeight = 0.0f;
    // 애니메이션 관련 혼합 작업
    // PivotBoneName을 1을 주면 랙돌만 켜짐, 0을 주면 애니메이션 켜짐
    // 즉 해당 부분은 애니메이션 상태
    //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

    // 0: 랙돌 끄기, 1: 랙돌 키기
    TargetRagDollBlendWeight = 0.0f;
    CurrentRagDollBlendWeight = 1.0f;
    bIsNowRagdollBlending = true;

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
    // Owner PC에서만 작동
    //FName WeaponSocket(TEXT("WeaponSocket"));
    //if (true == GetMesh()->DoesSocketExist(WeaponSocket) && false == IsValid(WeaponInstance))
    //{
    //    WeaponInstance = GetWorld()->SpawnActor<ASWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
    //    if (true == IsValid(WeaponInstance))
    //    {
    //        WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
    //    }

    //    // 레이어 연결(무기O)
    //    TSubclassOf<UAnimInstance> RifleCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
    //    if (true == IsValid(RifleCharacterAnimLayer))
    //    {
    //        GetMesh()->LinkAnimClassLayers(RifleCharacterAnimLayer);
    //    }

    //    // 무기 장착시 몽타주 실행
    //    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    //    if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance->GetEquipAnimMontage()))
    //    {
    //        AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
    //    }
    //}

    SpawnWeaponInstance_Server();

}

// 해제 했을 때 처리
void ASViewCharacter::InputQuickSlot02(const FInputActionValue& InValue)
{
    if (true == IsValid(WeaponInstance))
    {
        // Owner PC에서만 작동
        //// 레이어 연결(무기X)
        //TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
        //if (true == IsValid(UnarmedCharacterAnimLayer))
        //{
        //    GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
        //}

        //// 무기 해제시 몽타주 실행
        //USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        //if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance->GetUnequipAnimMontage()))
        //{
        //    AnimInstance->Montage_Play(WeaponInstance->GetUnequipAnimMontage());
        //}

        //WeaponInstance->Destroy();
        //WeaponInstance = nullptr;

        DestroyWeaponInstance_Server();

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

    //USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    //checkf(true == IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

    //if (true == IsValid(WeaponInstance))
    //{
    //    if (true == IsValid(WeaponInstance->GetMeleeAttackMontage()))
    //    {
    //        /*AnimInstance->PlayAnimMontage(WeaponInstance->GetMeleeAttackMontage());

    //        GetCharacterMovement()->SetMovementMode(MOVE_None);
    //        bIsNowAttacking = true;*/

    //        // 첫 콤보
    //        if (0 == CurrentComboCount) BeginAttack();
    //        // 콤보공격
    //        else
    //        {
    //            ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
    //            bIsAttackKeyPressed = true;
    //        }
    //    }
    //}

    // 단발
    if (false == bIsTriggerToggle) TryFire();

}

void ASViewCharacter::Menu(const FInputActionValue& InValue)
{
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASViewCharacter::Menu() has been called.")));

    ASPlayerController* PlayerController = GetController<ASPlayerController>();
    if (true == IsValid(PlayerController)) PlayerController->ToggleInGameMenu();

}

void ASViewCharacter::StartIronSight(const FInputActionValue& InValue)
{
    TargetFOV = 45.0f;

}

void ASViewCharacter::EndIronSight(const FInputActionValue& InValue)
{
    TargetFOV = 70.0f;

}

void ASViewCharacter::ToggleTrigger(const FInputActionValue& InValue)
{
    bIsTriggerToggle = !bIsTriggerToggle;

}

void ASViewCharacter::StartFire(const FInputActionValue& InValue)
{
    // 연발
    if (true == bIsTriggerToggle)
    {
        // 타이머로 발사
        GetWorldTimerManager().SetTimer(BetweenShotsTimer, this, &ThisClass::TryFire, TimeBetweenFire, true);
    }

}

void ASViewCharacter::StopFire(const FInputActionValue& InValue)
{
    // 타이머 정리
    GetWorldTimerManager().ClearTimer(BetweenShotsTimer);

}

void ASViewCharacter::SpawnLandMine(const FInputActionValue& InValue)
{
    /*if (true == IsValid(LandMineClass))
    {
        FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.0f) - FVector(0.0f, 0.0f, 90.0f);
        ASLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
        SpawnedLandMine->SetOwner(GetController());

    }*/

    // 호출은 Owning Client, 수행은 Server
    SpawnLandMine_Server();

}

void ASViewCharacter::TryFire()
{
    APlayerController* PlayerController = GetController<APlayerController>();
    if (true == IsValid(PlayerController) && true == IsValid(WeaponInstance))
    {
#pragma region CaculateTargetTransform
        float FocalDistance = 400.0f;       // 스프링암의 길이라고 생각하면 됨
        FVector FocalLocation;
        FVector CameraLocation;
        FRotator CameraRotation;

        // 컨트롤러에서 카메라의 위치와 회전 정보를 가져와 대입
        PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

        // 카메라 회전에 대한 단위 벡터(단위 방향 벡터)
        FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
        // 최종 초점 위치(== 눈 위치), 총 정보 반영X
        FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

        FVector WeaponMuzzleLocation = WeaponInstance->GetMesh()->GetSocketLocation(TEXT("MuzzleFlash"));
        // WeaponMuzzleLocation - FocalLocation: Focal에서 WeaponMuzzle로 가는 벡터
        // |: 벡터끼리 할 경우 내적을 뜻함
        // (WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera): 스칼라 값, 즉 길이
        // 구한 스칼라 값 * AimDirectionFromCamera: 해당 길이(스칼라)에 대한 단위 방향 벡터
        // FocalLocation까지 더한다면 눈 + 무기 정보 => 최종 초점 위치(FinalFocalLocation)
        FVector FinalFocalLocation = FocalLocation + (((WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera) * AimDirectionFromCamera);

        // 이를 토대로 얻은 값들을 최종 타겟 트랜스폼으로 지정
        FTransform TargetTransform = FTransform(CameraRotation, FinalFocalLocation);

        if (1 == ShowAttackDebug)
        {
            DrawDebugSphere(GetWorld(), WeaponMuzzleLocation, 2.0f, 16, FColor::Red, false, 60.0f);

            DrawDebugSphere(GetWorld(), CameraLocation, 2.0f, 16, FColor::Yellow, false, 60.0f);

            DrawDebugSphere(GetWorld(), FinalFocalLocation, 2.0f, 16, FColor::Magenta, false, 60.0f);

            // (WeaponLoc - FocalLoc)
            DrawDebugLine(GetWorld(), FocalLocation, WeaponMuzzleLocation, FColor::Yellow, false, 60.0f, 0, 2.0f);

            // AimDir
            DrawDebugLine(GetWorld(), CameraLocation, FinalFocalLocation, FColor::Blue, false, 60.0f, 0, 2.0f);

            // Project Direction Line
            DrawDebugLine(GetWorld(), WeaponMuzzleLocation, FinalFocalLocation, FColor::Red, false, 60.0f, 0, 2.0f);
        }

#pragma endregion

#pragma region PerformLineTracing

        // GetUnitAxis(EAxis::X): 포워드 벡터
        FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);
        FVector StartLocation = TargetTransform.GetLocation();
        FVector EndLocation = StartLocation + BulletDirection * WeaponInstance->GetMaxRange();

        FHitResult HitResult;
        FCollisionQueryParams TraceParams(NAME_None, false, this);
        TraceParams.AddIgnoredActor(WeaponInstance);

        // 관통X
        bool IsCollided = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel2, TraceParams);
        // 충돌된 것이 없는 경우
        if (false == IsCollided)
        {
            HitResult.TraceStart = StartLocation;
            HitResult.TraceEnd = EndLocation;
        }

        if (2 == ShowAttackDebug)
        {
            if (true == IsCollided)
            {
                DrawDebugSphere(GetWorld(), StartLocation, 2.0f, 16, FColor::Red, false, 60.0f);

                DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 2.0f, 16, FColor::Green, false, 60.0f);

                DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 60.0f, 0, 2.0f);
            }
            else
            {
                DrawDebugSphere(GetWorld(), StartLocation, 2.0f, 16, FColor::Red, false, 60.0f);

                DrawDebugSphere(GetWorld(), EndLocation, 2.0f, 16, FColor::Green, false, 60.0f);

                DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 60.0f, 0, 2.0f);
            }
        }

#pragma endregion

        if (true == IsCollided)
        {
            ASCharacter* HittedCharacter = Cast<ASCharacter>(HitResult.GetActor());
            if (true == IsValid(HittedCharacter))
            {
                FDamageEvent DamageEvent;
                //HittedCharacter->TakeDamage(10.0f, DamageEvent, GetController(), this);

                // 기존 작업: BoneNameString 출력 확인하면 None으로 뜸 -> 메시 기준이 아닌 캡슐 컴포넌트에 충돌되고 있기 때문
                // 수정 작업: SCharacter Attack을 무시하고 CharacterMesh에서 블록되게 설정하여 좀 더 세밀한 부위 판정으로 되게끔 변경
                FString BoneNameString = HitResult.BoneName.ToString();
                //UKismetSystemLibrary::PrintString(this, BoneNameString);
                //DrawDebugSphere(GetWorld(), HitResult.Location, 3.0f, 16, FColor(255, 0, 0, 255), true, 20.0f, 0U, 5.0f);

                // 헤드샷 대미지
                if (true == BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))
                {
                    HittedCharacter->TakeDamage(100.0f, DamageEvent, GetController(), this);
                }
                // 일반 피격 대미지
                else
                {
                    HittedCharacter->TakeDamage(10.0f, DamageEvent, GetController(), this);
                }
            }
        }

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance))
        {
            if (false == AnimInstance->Montage_IsPlaying(WeaponInstance->GetFireAnimMontage()))
            {
                AnimInstance->Montage_Play(WeaponInstance->GetFireAnimMontage());
            }
        }

        if (true == IsValid(FireShake))
        {
            PlayerController->ClientStartCameraShake(FireShake);
        }

    }
}
