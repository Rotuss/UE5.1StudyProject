// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SPlayerController.h"
#include "Game/SPlayerState.h"
#include "Game/SGameMode.h"
#include "UI/SHUD.h"
#include "UI/SGameResultWidget.h"
#include "Blueprint/UserWidget.h"
#include "Component/SStatComponent.h"
#include "Character/SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

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

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, NotificationText);

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

// ���� ������ ĳ���Ͱ� �׾�����
void ASPlayerController::OnOwningCharacterDead()
{
    ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(this));
    // ���� ������ ĳ���Ͱ� �׾��ٸ� ���Ӹ�忡�� �˸���
    // ���������� ĳ���Ͱ� �׾��� �� ��Ʈ�ѷ��� ���� �ʾҴ�
    if (true == HasAuthority() && true == IsValid(GameMode)) GameMode->OnControllerDead(this);

}

void ASPlayerController::ShowWinnerUI_Implementation()
{
    // Ŭ���� ��
    if (false == HasAuthority())
    {
        if (true == IsValid(WinnerUIClass))
        {
            USGameResultWidget* WinnerUI = CreateWidget<USGameResultWidget>(this, WinnerUIClass);
            if (true == IsValid(WinnerUI))
            {
                WinnerUI->AddToViewport(3);
                // �����̴ϱ� ������ 1��, ���� ����� ���� �ʿ�X
                WinnerUI->RankingText->SetText(FText::FromString(TEXT("#01")));

                FInputModeUIOnly Mode;
                Mode.SetWidgetToFocus(WinnerUI->GetCachedWidget());
                SetInputMode(Mode);

                bShowMouseCursor = true;
            }
        }
    }

}

void ASPlayerController::ShowLooserUI_Implementation(int32 InRanking)
{
    if (false == HasAuthority())
    {
        if (true == IsValid(LooserUIClass))
        {
            USGameResultWidget* LooserUI = CreateWidget<USGameResultWidget>(this, LooserUIClass);
            if (true == IsValid(LooserUI))
            {
                LooserUI->AddToViewport(3);
                FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
                LooserUI->RankingText->SetText(FText::FromString(RankingString));

                FInputModeUIOnly Mode;
                Mode.SetWidgetToFocus(LooserUI->GetCachedWidget());
                SetInputMode(Mode);

                bShowMouseCursor = true;
            }
        }
    }

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

    // ������ Ŭ�� ���� �۵� => UI ���� �߻�
    // OwningClient: ��(�� ����, �÷��̾�) ��ǻ��
    // OtherClient: OwningClient�� ������ �ٸ� ��ǻ�͵�, ���� ��ǻ��
    // �� UI�� �����׸� ���̸� ��, ���� UI�� OwningClient������ ������ ��
    // ���� ������ �����ִ� �۾�
    if (true == HasAuthority()) return;

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

    if (true == IsValid(CrosshairUIClass))
    {
        UUserWidget* CrosshairUIInstance = CreateWidget<UUserWidget>(this, CrosshairUIClass);
        if (true == IsValid(CrosshairUIInstance))
        {
            CrosshairUIInstance->AddToViewport(1);
            CrosshairUIInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (true == IsValid(NotificationTextUI))
    {
        UUserWidget* NewNotificationTextUI = CreateWidget<UUserWidget>(this, NotificationTextUI);
        if (true == IsValid(NewNotificationTextUI))
        {
            NewNotificationTextUI->AddToViewport(1);
            NewNotificationTextUI->SetVisibility(ESlateVisibility::Visible);
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
