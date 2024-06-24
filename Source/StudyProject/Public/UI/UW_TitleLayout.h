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
	// AddToViewport �� �� ȣ��
	virtual void NativeConstruct() override;

	// ���� ������ �� �ݹ� �Լ�
	UFUNCTION()
	void OnPlayButtonClicked();

	// ���� ������ �� �ݹ� �Լ�
	UFUNCTION()
	void OnExitButtonClicked();

private:
	// ���� ��ư
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> PlayButton;

	// ���� ��ư
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> ExitButton;

};
