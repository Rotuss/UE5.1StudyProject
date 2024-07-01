// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SUIPlayerController.generated.h"

/**
 * 
 */
// Ÿ��Ʋ, �κ�� ���� �������� UI�� ����� ���̱� ������ �θ��� Ŭ������ �ʿ�
UCLASS()
class STUDYPROJECT_API ASUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void JoinServer(const FString& InIPAddress);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASUIPlayerController", meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ASUIPlayerController", meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

};
