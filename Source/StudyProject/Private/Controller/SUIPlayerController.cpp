// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SUIPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (true == IsValid(UIWidgetClass))
	{
		// CreateWidget()�� ȣ��� �� UIWidgetInstance->NativeOnInitialize() �Լ��� ȣ��
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (true == IsValid(UIWidgetInstance))
		{
			// AddToViewport()�� ȣ�� �� �� UIWidgetInstance->NativeConstruct() �Լ��� ȣ��
			UIWidgetInstance->AddToViewport();

			// UI�� ����� ���̹Ƿ� FInputModeUIOnly
			FInputModeUIOnly Mode;
			// UIWidgetInstance���� ��Ŀ���� ������ ����
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

}

// �޾ƿ� InIPAddress�� ����
void ASUIPlayerController::JoinServer(const FString& InIPAddress)
{
	// NextLevel=%s => �Է� ���� IP�� ���ڴ�
	// Saved=false => ������ ���� �ҷ� �� ���ΰ� ������� ���� ���ΰ�
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString::Printf(TEXT("NextLevel=%s?Saved=false"), *InIPAddress));

}
