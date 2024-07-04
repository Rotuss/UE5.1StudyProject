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

// == Tick함수
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

    // 메뉴가 켜져있지 않으면
    if (false == bIsInGameMenuOn)
    {
        InGameMenuInstance->SetVisibility(ESlateVisibility::Visible);

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(InGameMenuInstance->GetCachedWidget());
        SetInputMode(Mode);

        // 만약 게임 일시 정지를 원할 때
        // input에 문제가 될 경우, InputAction 에셋의 TriggerWhenPaused 속성을 true로 지정해야 Pause 상태에서도 해당 입력 액션이 동작
        // SetPause(true); 

        bShowMouseCursor = true;
    }
    // 켜져있는 메뉴 끄려면
    else
    {
        InGameMenuInstance->SetVisibility(ESlateVisibility::Collapsed);

        FInputModeGameOnly InputModeGameOnly;
        SetInputMode(InputModeGameOnly);

        // 만약 게임 일시 정지를 해제를 원할 때
        // SetPause(false); 

        bShowMouseCursor = false;
    }

    bIsInGameMenuOn = !bIsInGameMenuOn;
}

// 내가 소유한 캐릭터가 죽었는지
void ASPlayerController::OnOwningCharacterDead()
{
    ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(this));
    // 내가 소유한 캐릭터가 죽었다면 게임모드에게 알리기
    // 실질적으로 캐릭터가 죽었을 뿐 컨트롤러는 죽지 않았다
    if (true == HasAuthority() && true == IsValid(GameMode)) GameMode->OnControllerDead(this);

}

void ASPlayerController::ShowWinnerUI_Implementation()
{
    // 클라일 때
    if (false == HasAuthority())
    {
        if (true == IsValid(WinnerUIClass))
        {
            USGameResultWidget* WinnerUI = CreateWidget<USGameResultWidget>(this, WinnerUIClass);
            if (true == IsValid(WinnerUI))
            {
                WinnerUI->AddToViewport(3);
                // 승자이니까 어차피 1등, 굳이 등수를 구할 필요X
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

// 빙의
void ASPlayerController::OnPossess(APawn* aPawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::OnPossess(ASPlayerPawn)"));
    Super::OnPossess(aPawn);
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::OnPossess(ASPlayerPawn)"));
}

// 빙의 탈출
void ASPlayerController::OnUnPossess()
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::OnUnPossess()"));
    Super::OnUnPossess();
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::OnUnPossess()"));
}

// 게임 시작
void ASPlayerController::BeginPlay()
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::BeginPlay()"));
    Super::BeginPlay();
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::BeginPlay()"));

    // 에디터에서 게임 플레이 시작시 마우스를 통해 추가 접속이 아닌 바로 진입 접속하게끔 설정
    FInputModeGameOnly InputModeGameOnly;
    SetInputMode(InputModeGameOnly);

    // 서버와 클라가 같이 작동 => UI 문제 발생
    // OwningClient: 내(현 유저, 플레이어) 컴퓨터
    // OtherClient: OwningClient을 제외한 다른 컴퓨터들, 서버 컴퓨터
    // 내 UI는 나한테만 보이면 됨, 따라서 UI는 OwningClient에서만 있으면 됨
    // 따라서 서버는 막아주는 작업
    if (true == HasAuthority()) return;

    if (true == IsValid(HUDWidgetClass))
    {
        // HUD 위젯 생성
        HUDWidget = CreateWidget<USHUD>(this, HUDWidgetClass);
        if (true == IsValid(HUDWidget))
        {
            // 화면에 띄우기, 안하면 화면에 안 뜸
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

    // InGameMenuClass가 유효하다면
    if (true == IsValid(InGameMenuClass))
    {
        // 위젯 생성(이니셜라이즈 호출)
        InGameMenuInstance = CreateWidget<UUserWidget>(this, InGameMenuClass);
        if (true == IsValid(InGameMenuInstance))
        {
            // 컨스트럭트 호출
            // 3: 상위에 띄움 => 숫자가 크면 클수록 위쪽에 띄워짐
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

// 게임 종료
void ASPlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::EndPlay()"));
    Super::EndPlay(EndPlayReason);
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::EndPlay()"));
}
