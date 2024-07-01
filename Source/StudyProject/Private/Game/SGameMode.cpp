// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameMode.h"
#include "Controller/SPlayerController.h"
#include "Character/SPlayerPawn.h"
#include "Game/SPlayerState.h"

ASGameMode::ASGameMode()
{
	PlayerControllerClass = ASPlayerController::StaticClass();
	DefaultPawnClass = ASPlayerPawn::StaticClass();
}

// 게임 시작 초기화 부분 호출
void ASGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        InitGame()"));
	Super::InitGame(MapName, Options, ErrorMessage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        InitGame()"));
}

// state는 데디케이티드 서버할 때 명확해짐
// 현재는 이런게 존재한다 정도도만 인지하면 됨
// 게임모드의 경우, 승패 조건 및 게임의 연장, 중단
// 게임스테이트는 현재 플레이어가 얼마나 남았는지 전체적인 현재 상황 등
void ASGameMode::InitGameState()
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        InitGameState()"));
	Super::InitGameState();
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        InitGameState()"));
}

// 컴포넌트들이 모두 초기화 된 후 작업
void ASGameMode::PostInitializeComponents()
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PostInitializeComponents()"));
	Super::PostInitializeComponents();
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PostInitializeComponents()"));
}

// 로그인 전
void ASGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMassage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PreLogin()"));
	Super::PreLogin(Options, Address, UniqueId, ErrorMassage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PreLogin()"));
}

// 로그인 중
// UPlayer* NewPlayer 현재 인원과 같음
APlayerController* ASGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        Login()"));
	APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        Login()"));

	return PlayerController;
}

// 로그인 후
void ASGameMode::PostLogin(APlayerController* NewPlayer)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PostLogin(ASPlayerController)"));
	Super::PostLogin(NewPlayer);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PostLogin(ASPlayerController)"));

	ASPlayerState* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>();
	if (true == IsValid(PlayerState))
	{
		PlayerState->InitPlayerState();
	}

	ASPlayerController* NewPlayerController = Cast<ASPlayerController>(NewPlayer);
	if (true == IsValid(NewPlayerController))
	{
		AlivePlayerControllers.Add(NewPlayerController);
		// 로그인 후 서버 접속 표시 알림
		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}

}

void ASGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ASPlayerController* ExitingPlayerController = Cast<ASPlayerController>(Exiting);
	if (true == IsValid(ExitingPlayerController) && INDEX_NONE != AlivePlayerControllers.Find(ExitingPlayerController))
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);
	}

}
