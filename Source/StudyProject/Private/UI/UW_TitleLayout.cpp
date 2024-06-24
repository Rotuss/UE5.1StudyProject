// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_TitleLayout.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// AddToViewport �� �� ���ε� �۾�
void UUW_TitleLayout::NativeConstruct()
{
	// �ݹ� �Լ� ����
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);

}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Example"));

}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);

}
