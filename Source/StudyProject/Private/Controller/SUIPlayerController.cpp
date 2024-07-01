// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SUIPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (true == IsValid(UIWidgetClass))
	{
		// CreateWidget()이 호출될 때 UIWidgetInstance->NativeOnInitialize() 함수가 호출
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (true == IsValid(UIWidgetInstance))
		{
			// AddToViewport()가 호출 될 때 UIWidgetInstance->NativeConstruct() 함수가 호출
			UIWidgetInstance->AddToViewport();

			// UI만 사용할 것이므로 FInputModeUIOnly
			FInputModeUIOnly Mode;
			// UIWidgetInstance으로 포커싱이 잡히게 설정
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

}

// 받아온 InIPAddress로 열기
void ASUIPlayerController::JoinServer(const FString& InIPAddress)
{
	// NextLevel=%s => 입력 받은 IP로 열겠다
	// Saved=false => 저장한 것을 불러 올 것인가 사용하지 않을 것인가
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString::Printf(TEXT("NextLevel=%s?Saved=false"), *InIPAddress));

}
