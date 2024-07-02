// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

// 실행 흐름 순서 알아 놓으면 좋을 것
//LogTemp: Error:          Start ASGameMode::        InitGame()
//LogTemp: Error:          End   ASGameMode::        InitGame()
//LogTemp: Error:          Start ASGameMode::        InitGameState()
//LogTemp: Error:          End   ASGameMode::        InitGameState()
//LogTemp: Error:          Start ASGameMode::        PostInitializeComponents()
//LogTemp: Error:          End   ASGameMode::        PostInitializeComponents()
//LogTemp: Error:          Start ASGameMode::        Login()
//LogTemp: Warning:        Start ASPlayerController::PostInitializeComponents()
//LogTemp: Warning:        End   ASPlayerController::PostInitializeComponents()
//LogTemp: Error:          End   ASGameMode::        Login()
//LogTemp: Warning:        Start ASPlayerController::SetupInputComponent()
//LogTemp: Warning:        End   ASPlayerController::SetupInputComponent()
//LogTemp: Error:          Start ASGameMode::        PostLogin(ASPlayerController)
//LogTemp:                 Start ASPlayerPawn::      PostInitializeComponents()
//LogTemp:                 End   ASPlayerPawn::      PostInitializeComponents()
//LogTemp: Warning:        Start ASPlayerController::OnPossess(ASPlayerPawn)
//LogTemp:                 Start ASPlayerPawn::      PossessedBy(ASPlayerController)
//LogTemp:                 End   ASPlayerPawn::      PossessedBy(ASPlayerController)
//LogTemp:                 Start ASPlayerPawn::      SetupPlayerInputComponent(PlayerInputComponent)
//LogTemp:                 End   ASPlayerPawn::      SetupPlayerInputComponent(PlayerInputComponent)
//LogTemp: Warning:        End   ASPlayerController::OnPossess(ASPlayerPawn)
//LogTemp: Error:          End   ASGameMode::        PostLogin(ASPlayerController)
//LogTemp: Warning:        Start ASPlayerController::BeginPlay()
//LogTemp: Warning:        End   ASPlayerController::BeginPlay()
//LogTemp:                 Start ASPlayerPawn::      BeginPlay()
//LogTemp:                 End   ASPlayerPawn::      BeginPlay()
//LogTemp: Warning:        Start ASPlayerController::PlayerTick()
//LogTemp: Warning:        End   ASPlayerController::PlayerTick()
//LogTemp:                 Start ASPlayerPawn::      Tick()
//LogTemp:                 End   ASPlayerPawn::      Tick()
//LogTemp: Warning:        Start ASPlayerController::OnUnPossess()
//LogTemp:                 Start ASPlayerPawn::      UnPossessed()
//LogTemp:                 End   ASPlayerPawn::      UnPossessed()
//LogTemp: Warning:        End   ASPlayerController::OnUnPossess()
//LogTemp: Warning:        Start ASPlayerController::EndPlay()
//LogTemp: Warning:        End   ASPlayerController::EndPlay()
//LogTemp:                 Start ASPlayerPawn::      EndPlay()
//LogTemp:                 End   ASPlayerPawn::      EndPlay()

UENUM(BlueprintType)
enum class EMatchState : uint8
{
    None,
    Waiting,
    Playing,
    Ending,
    End,

};

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    virtual void InitGameState() override;

    virtual void PostInitializeComponents() override;

    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMassage) override;

    virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

    virtual void PostLogin(APlayerController* NewPlayer) override;

    virtual void BeginPlay() override;

    virtual void Logout(AController* Exiting) override;

private:
    UFUNCTION()
    void OnMainTimerElapsed();

    void NotifyToAllPlayer(const FString& NotificationString);

public:
    // 우선 매칭 상태에서 15초 가량 기다리게 하기 위함
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASGameMode")
    int32 WaitingTime = 15;

    FTimerHandle MainTimerHandle;

    EMatchState MatchState = EMatchState::Waiting;

    // 플레이 되기 전까지 남은 시간
    int32 RemainWaitingTimeForPlaying = 15;

    // 최소 필요한 플레이어 수
    int32 MinimumPlayerCountForPlaying = 2;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASGameMode", meta = (AllowPrivateAccess))
    TArray<TObjectPtr<class ASPlayerController>> AlivePlayerControllers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASGameMode", meta = (AllowPrivateAccess))
    TArray<TObjectPtr<class ASPlayerController>> DeadPlayerControllers;

};
