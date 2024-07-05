// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SGameResultWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void USGameResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 델리게이트 연결 작업
    if (false == ReturnToLobbyButton.Get()->OnClicked.IsAlreadyBound(this, &ThisClass::OnReturnToLobbyButtonClicked))
    {
        ReturnToLobbyButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnReturnToLobbyButtonClicked);
    }

}

void USGameResultWidget::OnReturnToLobbyButtonClicked()
{
    // 로비레벨로 보내는 작업
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby?Saved=false")));

}
