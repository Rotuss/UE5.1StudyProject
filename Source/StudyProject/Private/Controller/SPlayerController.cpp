// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SPlayerController.h"
#include "Game/SPlayerState.h"
#include "UI/SHUD.h"
#include "Blueprint/UserWidget.h"
#include "Component/SStatComponent.h"
#include "Character/SCharacter.h"

ASPlayerController::ASPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

// 
void ASPlayerController::PostInitializeComponents()
{
	//UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::PostInitializeComponents()"));
	Super::PostInitializeComponents();
	//UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::PostInitializeComponents()"));
}

// == Tick�Լ�
void ASPlayerController::PlayerTick(float DeltaSeconds)
{
    /*static bool bOnce = false;
    if (false == bOnce)
    {
        UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::PlayerTick()"));
    }*/
    Super::PlayerTick(DeltaSeconds);
    /*if (false == bOnce)
    {
        UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::PlayerTick()"));
        bOnce = true;
    }*/
}

void ASPlayerController::ToggleInGameMenu()
{
    checkf(true == IsValid(InGameMenuInstance), TEXT("Invalid InGameMenuInstance"));

    // �޴��� �������� ������
    if (false == bIsInGameMenuOn)
    {
        InGameMenuInstance->SetVisibility(ESlateVisibility::Visible);

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(InGameMenuInstance->GetCachedWidget());
        SetInputMode(Mode);

        // ���� ���� �Ͻ� ������ ���� ��
        // input�� ������ �� ���, InputAction ������ TriggerWhenPaused �Ӽ��� true�� �����ؾ� Pause ���¿����� �ش� �Է� �׼��� ����
        // SetPause(true); 

        bShowMouseCursor = true;
    }
    // �����ִ� �޴� ������
    else
    {
        InGameMenuInstance->SetVisibility(ESlateVisibility::Collapsed);

        FInputModeGameOnly InputModeGameOnly;
        SetInputMode(InputModeGameOnly);

        // ���� ���� �Ͻ� ������ ������ ���� ��
        // SetPause(false); 

        bShowMouseCursor = false;
    }

    bIsInGameMenuOn = !bIsInGameMenuOn;
}

void ASPlayerController::SetupInputComponent()
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::SetupInputComponent()"));
    Super::SetupInputComponent();
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::SetupInputComponent()"));
}

// ����
void ASPlayerController::OnPossess(APawn* aPawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::OnPossess(ASPlayerPawn)"));
    Super::OnPossess(aPawn);
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::OnPossess(ASPlayerPawn)"));
}

// ���� Ż��
void ASPlayerController::OnUnPossess()
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::OnUnPossess()"));
    Super::OnUnPossess();
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::OnUnPossess()"));
}

// ���� ����
void ASPlayerController::BeginPlay()
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::BeginPlay()"));
    Super::BeginPlay();
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::BeginPlay()"));

    // �����Ϳ��� ���� �÷��� ���۽� ���콺�� ���� �߰� ������ �ƴ� �ٷ� ���� �����ϰԲ� ����
    FInputModeGameOnly InputModeGameOnly;
    SetInputMode(InputModeGameOnly);

    if (true == IsValid(HUDWidgetClass))
    {
        // HUD ���� ����
        HUDWidget = CreateWidget<USHUD>(this, HUDWidgetClass);
        if (true == IsValid(HUDWidget))
        {
            // ȭ�鿡 ����, ���ϸ� ȭ�鿡 �� ��
            HUDWidget->AddToViewport();

            ASPlayerState* SPlayerState = GetPlayerState<ASPlayerState>();
            if (true == IsValid(SPlayerState))
            {
                HUDWidget->BindPlayerState(SPlayerState);
            }

            ASCharacter* PC = GetPawn<ASCharacter>();
            if (true == IsValid(PC))
            {
                USStatComponent* StatComponent = PC->GetStatComponent();
                if (true == IsValid(StatComponent))
                {
                    HUDWidget->BindStatComponent(StatComponent);
                }
            }
        }
    }

    // InGameMenuClass�� ��ȿ�ϴٸ�
    if (true == IsValid(InGameMenuClass))
    {
        // ���� ����(�̴ϼȶ����� ȣ��)
        InGameMenuInstance = CreateWidget<UUserWidget>(this, InGameMenuClass);
        if (true == IsValid(InGameMenuInstance))
        {
            // ����Ʈ��Ʈ ȣ��
            // 3: ������ ��� => ���ڰ� ũ�� Ŭ���� ���ʿ� �����
            InGameMenuInstance->AddToViewport(3);
            InGameMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

}

// ���� ����
void ASPlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::EndPlay()"));
    Super::EndPlay(EndPlayReason);
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::EndPlay()"));
}
