// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USGameResultWidget", meta = (BindWidget))
	TObjectPtr<class UTextBlock> RankingText;

};
