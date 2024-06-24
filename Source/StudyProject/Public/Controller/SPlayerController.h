// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    ASPlayerController();
    
public:
    virtual void PostInitializeComponents() override;

    virtual void PlayerTick(float DeltaSeconds) override;

    class USHUD* GetHUDWidget() const { return HUDWidget; };

    void ToggleInGameMenu();

protected:
    virtual void SetupInputComponent() override;

    virtual void OnPossess(APawn* aPawn) override;

    virtual void OnUnPossess() override;

    virtual void BeginPlay() override;

    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY();
    TObjectPtr<class USHUD> HUDWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess));
    TSubclassOf<class USHUD> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", meta = (AllowPrivateAccess))
    TSubclassOf<UUserWidget> InGameMenuClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASPlayerController", meta = (AllowPrivateAccess))
    TObjectPtr<UUserWidget> InGameMenuInstance;

    bool bIsInGameMenuOn = false;

};
