// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_TitleLayout.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// AddToViewport 될 때 바인드 작업
void UUW_TitleLayout::NativeConstruct()
{
	// 콜백 함수 연결
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);

}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("Example"));
	// NextLevel이 Key, Example가 Value
	// 그럼 Loading 레벨에서는 NextLevel을 파싱해서 Example 값을 얻어내면 됨
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Example")));

}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);

}
