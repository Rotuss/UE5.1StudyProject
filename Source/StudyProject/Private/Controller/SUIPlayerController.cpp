// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SUIPlayerController.h"
#include "Blueprint/UserWidget.h"

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
