// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TitleLayout.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API UUW_TitleLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_TitleLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// AddToViewport 될 때 호출
	virtual void NativeConstruct() override;

	// 시작 눌렸을 때 콜백 함수
	UFUNCTION()
	void OnPlayButtonClicked();

	// 종료 눌렸을 때 콜백 함수
	UFUNCTION()
	void OnExitButtonClicked();

private:
	// 시작 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> PlayButton;

	// 종료 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> ExitButton;

};
