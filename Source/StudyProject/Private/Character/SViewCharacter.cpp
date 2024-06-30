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
#include "SViewCharacterSettings.h"             // ��⿡ �߰������ ������ ����

//int32 ASViewCharacter::ShowAttackDebug = 0;
//FAutoConsoleVariableRef CVarShowAttackDebug(TEXT("StudyProject.ShowAttackDebug"), ASViewCharacter::ShowAttackDebug, TEXT(""), ECVF_Cheat);

ASViewCharacter::ASViewCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    // AutoActivate: �����Ǹ� �ٷ� �����µ� ���� �� Ű�� �� �� �ְԲ� ����� false�� ����
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
        // subsystem�� ���� �̱��� ��ü�� �����´�
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (true == IsValid(Subsystem))
        {
            Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
        }
    }

    //// CDO ��������
    //const USViewCharacterSettings* CDO = GetDefault<USViewCharacterSettings>();
    //// 0 ~ ������ ������ŭ �� �ϳ��� �������� ����
    //int32 RandIndex = FMath::RandRange(0, CDO->PlayerCharacterMeshMaterialPaths.Num() - 1);
    //CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[RandIndex];
    //// RequestAsyncLoad: �񵿱� ��û
    //// CurrentPlayerCharacterMeshMaterialPath ��ζ� FStreamableDelegate::CreateLambda ��������Ʈ ���ٸ� ����Ͽ� �񵿱� ��û
    //AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
    //    CurrentPlayerCharacterMeshMaterialPath,
    //    FStreamableDelegate::CreateLambda([this]() -> void
    //        {
    //            // �ڵ� ����
    //            AssetStreamableHandle->ReleaseHandle();
    //            // ���׸��� ���
    //            TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
    //            // ��µ� �����Ͽ� ��ȿ�� ���̸�
    //            if (true == LoadedMaterialInstanceAsset.IsValid())
    //            {
    //                // �ش� ���׸���� ����
    //                // 0���� ������ �ٵ� �����ϱ� ����
    //                GetMesh()->SetMaterial(0, LoadedMaterialInstanceAsset.Get());
    //            }
    //        })
    //);

    // Ŭ�󿡼��� ����ǰ� BeginPlay���� ����
    SetViewMode(EViewMode::TPSView);

}

// ���������� ���ư��� ����
void ASViewCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // ���� ����
    //SetViewMode(EViewMode::BackView);
    //DestArmLength = 400.0f; // �ʱ�ȭ���� �� �� �����ؾ� ��
    //DestArmRotation = FRotator::ZeroRotator; // ��信���� �ǹ̰� �����Ƿ�
    //SetViewMode(EViewMode::TPSView);         // ���� Ŭ�󿡼��� ���ư��� �ʰ� ��
}

void ASViewCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // �� ��ȯ �ڿ��������� ���� Tick���� ����
    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 35.0f);
    CameraComponent->SetFieldOfView(CurrentFOV);

    // ��Ʈ�ѷ��� ��ȿ�ϴٸ�
    if (true == IsValid(GetController()))
    {
        // �ش� ��Ʈ�ѷ� ���� ���� => �ִϸ��̼� ȸ�� ������ ���� �۾�
        /*FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;*/

        PreviousAimPitch = CurrentAimPitch;
        PreviousAimYaw = CurrentAimYaw;

        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;

    }

    // Pitch Ȥ�� Yaw ���� �޶����� �� �������� ó���Ͽ� Ŭ�� �Ѹ��� �۾�
    if (PreviousAimPitch != CurrentAimPitch || PreviousAimYaw != CurrentAimYaw)
    {
        if (false == HasAuthority())
        {
            UpdateAimValue_Server(CurrentAimPitch, CurrentAimYaw);

        }

    }

    // �յ� Ȥ�� �¿� ���� �޶����� �� �������� ó���Ͽ� Ŭ�� �Ѹ��� �۾�
    if (PreviousForwardInputValue != ForwardInputValue || PreviousRightInputValue != RightInputValue)
    {
        if (false == HasAuthority())
        {
            UpdateInputValue_Server(ForwardInputValue, RightInputValue);

        }

    }

    // ���� �۾� bool ���� ���� �ִٸ�
    if (true == bIsNowRagdollBlending)
    {
        CurrentRagDollBlendWeight = FMath::FInterpTo(CurrentRagDollBlendWeight, TargetRagDollBlendWeight, DeltaTime, 10.0f);

        FName PivotBoneName = FName(TEXT("spine_01"));
        // ���� ���� ��� ���� ������ �۵� ��ų ���ΰ�
        GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagDollBlendWeight);

        // ���� ��ġ ���� ������ ���� 0�� ����� ��ġ(���� ��)�� �Ǹ� ���� ���� 
        if (KINDA_SMALL_NUMBER > CurrentRagDollBlendWeight - TargetRagDollBlendWeight)
        {
            GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
            bIsNowRagdollBlending = false;
        }

        // HP�� 0�� �����ٸ�
        if (true == IsValid(GetStatComponent()) && KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
        {
            // ��� ���� ���� ����ġ(������ ��ü�� �ִ� ���� ��ü�� root�� �����)
            GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("root")), 1.0f);
            GetMesh()->SetSimulatePhysics(true);
            // ���� Tick���� �ɸ��� �ʰ� false
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
        // DirectionToMove���� ���ߴ�?
        // SizeSquared�� ���밪���� ��� ���̸� ��Ÿ��
        if (KINDA_SMALL_NUMBER < DirectionToMove.SizeSquared())
        {
            // X�� ���� �����
            GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
            // �ش� �������� �̵�
            AddMovementInput(DirectionToMove);
            // 0���� �ʱ�ȭ
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
        // �����۾�
        SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, DestArmLength, DeltaTime, ArmLengthChangeSpeed);
        SpringArmComponent->SetRelativeRotation(FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), DestArmRotation, DeltaTime, ArmRotationChangeSpeed));
    }
}

float ASViewCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (false == IsValid(GetStatComponent())) return FinalDamage;

    // ���� HP�� 0�� ����� ���¶��
    if (KINDA_SMALL_NUMBER > GetStatComponent()->GetCurrentHP())
    {
        // ���� ���� ���� On
        GetMesh()->SetSimulatePhysics(true);
    }
    // �¾����� ���� HP�� �����ִٸ�
    else
    {
        // ��ü�� �������� �ϴ� �� �̸�
        FName PivotBoneName = FName(TEXT("spine_01"));
        // �� ������ ���� �ش� ���� �ùķ��̼� ������ Ű��
        GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);

        // ���� ��� ���� ġ�������Բ� ����ġ�� 1.0f�� ����
        // �� �ش� �κ��� ���� ����
        //float BlendWeight = 1.0f; 
        //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

        // 1: ���� Ű��, 0: ���� ����
        TargetRagDollBlendWeight = 1.0f;

        // Ÿ�̸� ���� ��������Ʈ ���ε� �۾�
        HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
        // Ÿ�̸� �ð��� �Ǹ� �۵��ؾ� �Ǵ� �Լ� ȣ��
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
    // ���� �����̸� ����
    if (CurrentViewMode == InViewMode)
    {
        return;
    }

    // ���� ������Ʈ
    CurrentViewMode = InViewMode;

    switch (CurrentViewMode)
    {
    case EViewMode::BackView:
        //SpringArmComponent->TargetArmLength = 400.0f;
        //// ControlRotation�� Pawn�� ȸ���� ����ȭ -> Pawn�� ȸ���� SprintArm�� ȸ�� ����ȭ. �̷� ���� SetRotation()�� ���ǹ�
        //SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        SpringArmComponent->bUsePawnControlRotation = true;
        SpringArmComponent->bDoCollisionTest = true;

        SpringArmComponent->bInheritPitch = true;
        SpringArmComponent->bInheritYaw = true;
        SpringArmComponent->bInheritRoll = false;

        // ȸ�����⿡ ���� �޽� ȸ��
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
        GetCharacterMovement()->bOrientRotationToMovement = true;   // ������ �ϴ� �������� ȸ���Ǵ°� ����ϰڴ�
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

        // 45���� ���ܼ� �����̴� ���� �ε巴�� �ٲٱ� ����
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

    // CDO ��������
    const USViewCharacterSettings* CDO = GetDefault<USViewCharacterSettings>();
    CurrentPlayerCharacterMeshMaterialPath = CDO->PlayerCharacterMeshMaterialPaths[TeamIdx];
    // RequestAsyncLoad: �񵿱� ��û
    // CurrentPlayerCharacterMeshMaterialPath ��ζ� FStreamableDelegate::CreateLambda ��������Ʈ ���ٸ� ����Ͽ� �񵿱� ��û
    AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        CurrentPlayerCharacterMeshMaterialPath,
        FStreamableDelegate::CreateLambda([this]() -> void
            {
                // �ڵ� ����
                AssetStreamableHandle->ReleaseHandle();
                // ���׸��� ���
                TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(CurrentPlayerCharacterMeshMaterialPath);
                // ��µ� �����Ͽ� ��ȿ�� ���̸�
                if (true == LoadedMaterialInstanceAsset.IsValid())
                {
                    // �ش� ���׸���� ����
                    // 0���� ������ �ٵ� �����ϱ� ����
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

// WithValidation ����
// Validate: ������ ���� ���� ���
bool ASViewCharacter::SpawnLandMine_Server_Validate()
{
    return true;
}

// Implementation: ���� ���� ���
void ASViewCharacter::SpawnLandMine_Server_Implementation()
{
    if (true == IsValid(LandMineClass))
    {
        FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 200.0f) - FVector(0.0f, 0.0f, 90.0f);
        ASLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
        SpawnedLandMine->SetOwner(GetController());
    }

}

// Ŭ�� �ƴ� �������� �̷����
void ASViewCharacter::SpawnWeaponInstance_Server_Implementation()
{
    FName WeaponSocket(TEXT("WeaponSocket"));
    if (true == GetMesh()->DoesSocketExist(WeaponSocket) && false == IsValid(WeaponInstance))
    {
        // ������ WeaponInstance �� ����
        // �ش� ���� ����� �� OnRep_WeaponInstance() ȣ��Ǹ� Replicated
        WeaponInstance = GetWorld()->SpawnActor<ASWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (true == IsValid(WeaponInstance))
        {
            WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
        }
    }

}

// Ŭ�� �ƴ� �������� �̷����
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
    // ������ �Ǿ��� ������ �� ���¿��� ������� ���Դ�
    if (true == IsValid(WeaponInstance))
    {
        // ������ �ִ� ���̾�� ����
        TSubclassOf<UAnimInstance> RifleCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
        if (true == IsValid(RifleCharacterAnimLayer))
        {
            GetMesh()->LinkAnimClassLayers(RifleCharacterAnimLayer);
        }

        // ���� ��Ÿ�� ����
        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance->GetEquipAnimMontage()))
        {
            AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
        }

        // ���̾� �� ��Ÿ�� ����
        UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
        UnequipAnimMontage = WeaponInstance->GetUnequipAnimMontage();

    }
    // ���� ���� ������ ��ġ�鼭 DestroyWeaponInstance_Server�� ���� ����� ����
    // ������ ������ ���� ���̾� �� ��Ÿ�ָ� ����
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
    // ��ü�� �������� �ϴ� �� �̸�
    FName PivotBoneName = FName(TEXT("spine_01"));
    // �� ������ ���� �ش� ���� �ùķ��̼� ������ ����
    //GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);

    //float BlendWeight = 0.0f;
    // �ִϸ��̼� ���� ȥ�� �۾�
    // PivotBoneName�� 1�� �ָ� ������ ����, 0�� �ָ� �ִϸ��̼� ����
    // �� �ش� �κ��� �ִϸ��̼� ����
    //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

    // 0: ���� ����, 1: ���� Ű��
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
    // Switch-Case ���� ������ Scope�� �����ϸ� �ش� Scope ������ ���� ������ ����
    { 
        // ī�޶� �ٶ󺸴� �������� ȸ���ϱ� ���� �ڵ�

        const FRotator ControlRotation = GetController()->GetControlRotation();
        const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.0f);

        // �ش� �� ����� ���� ���͸� �̿��� ���� ����
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

    //    // �¿�
    //    AddControllerYawInput(LookVector.X);
    //    // ����
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
        /* Case 1. ������ BackView �����̾��ٸ�

          BackView�� ��Ʈ�� ȸ���� == �������� ȸ����.
          �׷��� QuarterView�� ĳ������ ȸ���� == ��Ʈ�� ȸ����.
          ���� ���� ���� ���� ĳ������ ���� ȸ������ ��Ʈ�� ȸ���� �����ص־� ��.
          �ȱ׷��� ��Ʈ�� ȸ���� �Ͼ�鼭 ���� ĳ������ ȸ������ �������� ȸ����(��Ʈ�� ȸ����)���� ����ȭ��.
        */
        GetController()->SetControlRotation(GetActorRotation());
        DestArmLength = 900.0f;
        DestArmRotation = FRotator(-45.0f, 0.0f, 0.0f);
        SetViewMode(EViewMode::QuarterView);
        break;
    case EViewMode::QuarterView:
        /* Case 2. ������ QuarterView �����̾��ٸ�

          ��Ʈ�� ȸ���� ĳ���� ȸ���� ������ ��������.
          //QuarterView�� ���� ���������� ȸ������ ��Ʈ�� ȸ���� �����ص� ���¿��� ���� �����ؾ� �ùٸ�.
          BackView������ ��Ʈ�� �����̼��� ���� ȸ���� ����ȭ�ǰ� ���� ȸ���� ������ ���� ȸ���� ����ȭ.
          ���� ������ ���� ȸ���� ���Ƿ� ������ �� ����. 0���� ����.
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

// ���� ���� �� ������ ã�Ƽ� ����
void ASViewCharacter::InputQuickSlot01(const FInputActionValue& InValue)
{
    // Owner PC������ �۵�
    //FName WeaponSocket(TEXT("WeaponSocket"));
    //if (true == GetMesh()->DoesSocketExist(WeaponSocket) && false == IsValid(WeaponInstance))
    //{
    //    WeaponInstance = GetWorld()->SpawnActor<ASWeaponActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
    //    if (true == IsValid(WeaponInstance))
    //    {
    //        WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
    //    }

    //    // ���̾� ����(����O)
    //    TSubclassOf<UAnimInstance> RifleCharacterAnimLayer = WeaponInstance->GetArmedCharacterAnimLayer();
    //    if (true == IsValid(RifleCharacterAnimLayer))
    //    {
    //        GetMesh()->LinkAnimClassLayers(RifleCharacterAnimLayer);
    //    }

    //    // ���� ������ ��Ÿ�� ����
    //    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    //    if (true == IsValid(AnimInstance) && true == IsValid(WeaponInstance->GetEquipAnimMontage()))
    //    {
    //        AnimInstance->Montage_Play(WeaponInstance->GetEquipAnimMontage());
    //    }
    //}

    SpawnWeaponInstance_Server();

}

// ���� ���� �� ó��
void ASViewCharacter::InputQuickSlot02(const FInputActionValue& InValue)
{
    if (true == IsValid(WeaponInstance))
    {
        // Owner PC������ �۵�
        //// ���̾� ����(����X)
        //TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer = WeaponInstance->GetUnarmedCharacterAnimLayer();
        //if (true == IsValid(UnarmedCharacterAnimLayer))
        //{
        //    GetMesh()->LinkAnimClassLayers(UnarmedCharacterAnimLayer);
        //}

        //// ���� ������ ��Ÿ�� ����
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

    // ���� �� ����X
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

    //        // ù �޺�
    //        if (0 == CurrentComboCount) BeginAttack();
    //        // �޺�����
    //        else
    //        {
    //            ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
    //            bIsAttackKeyPressed = true;
    //        }
    //    }
    //}

    // �ܹ�
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
    // ����
    if (true == bIsTriggerToggle)
    {
        // Ÿ�̸ӷ� �߻�
        GetWorldTimerManager().SetTimer(BetweenShotsTimer, this, &ThisClass::TryFire, TimeBetweenFire, true);
    }

}

void ASViewCharacter::StopFire(const FInputActionValue& InValue)
{
    // Ÿ�̸� ����
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

    // ȣ���� Owning Client, ������ Server
    SpawnLandMine_Server();

}

void ASViewCharacter::TryFire()
{
    APlayerController* PlayerController = GetController<APlayerController>();
    if (true == IsValid(PlayerController) && true == IsValid(WeaponInstance))
    {
#pragma region CaculateTargetTransform
        float FocalDistance = 400.0f;       // ���������� ���̶�� �����ϸ� ��
        FVector FocalLocation;
        FVector CameraLocation;
        FRotator CameraRotation;

        // ��Ʈ�ѷ����� ī�޶��� ��ġ�� ȸ�� ������ ������ ����
        PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

        // ī�޶� ȸ���� ���� ���� ����(���� ���� ����)
        FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
        // ���� ���� ��ġ(== �� ��ġ), �� ���� �ݿ�X
        FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

        FVector WeaponMuzzleLocation = WeaponInstance->GetMesh()->GetSocketLocation(TEXT("MuzzleFlash"));
        // WeaponMuzzleLocation - FocalLocation: Focal���� WeaponMuzzle�� ���� ����
        // |: ���ͳ��� �� ��� ������ ����
        // (WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera): ��Į�� ��, �� ����
        // ���� ��Į�� �� * AimDirectionFromCamera: �ش� ����(��Į��)�� ���� ���� ���� ����
        // FocalLocation���� ���Ѵٸ� �� + ���� ���� => ���� ���� ��ġ(FinalFocalLocation)
        FVector FinalFocalLocation = FocalLocation + (((WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera) * AimDirectionFromCamera);

        // �̸� ���� ���� ������ ���� Ÿ�� Ʈ���������� ����
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

        // GetUnitAxis(EAxis::X): ������ ����
        FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);
        FVector StartLocation = TargetTransform.GetLocation();
        FVector EndLocation = StartLocation + BulletDirection * WeaponInstance->GetMaxRange();

        FHitResult HitResult;
        FCollisionQueryParams TraceParams(NAME_None, false, this);
        TraceParams.AddIgnoredActor(WeaponInstance);

        // ����X
        bool IsCollided = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel2, TraceParams);
        // �浹�� ���� ���� ���
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

                // ���� �۾�: BoneNameString ��� Ȯ���ϸ� None���� �� -> �޽� ������ �ƴ� ĸ�� ������Ʈ�� �浹�ǰ� �ֱ� ����
                // ���� �۾�: SCharacter Attack�� �����ϰ� CharacterMesh���� ��ϵǰ� �����Ͽ� �� �� ������ ���� �������� �ǰԲ� ����
                FString BoneNameString = HitResult.BoneName.ToString();
                //UKismetSystemLibrary::PrintString(this, BoneNameString);
                //DrawDebugSphere(GetWorld(), HitResult.Location, 3.0f, 16, FColor(255, 0, 0, 255), true, 20.0f, 0U, 5.0f);

                // ��弦 �����
                if (true == BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))
                {
                    HittedCharacter->TakeDamage(100.0f, DamageEvent, GetController(), this);
                }
                // �Ϲ� �ǰ� �����
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
