// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SInputConfigData.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> ChangeView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Jump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> QuickSlot01;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> QuickSlot02;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Menu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IronSight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> LandMine;

};
