// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SPlayerController.h"
#include "Game/SPlayerState.h"
#include "Component/SStatComponent.h"
#include "Character/SCharacter.h"
#include "UI/SHUD.h"

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

}

// 게임 종료
void ASPlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    //UE_LOG(LogTemp, Warning, TEXT("       Start ASPlayerController::EndPlay()"));
    Super::EndPlay(EndPlayReason);
    //UE_LOG(LogTemp, Warning, TEXT("       End   ASPlayerController::EndPlay()"));
}
