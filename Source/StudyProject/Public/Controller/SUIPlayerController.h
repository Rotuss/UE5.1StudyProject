// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SUIPlayerController.generated.h"

/**
 * 
 */
// 타이틀, 로비와 같은 레벨들은 UI로 사용할 것이기 때문에 부모인 클래스가 필요
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
