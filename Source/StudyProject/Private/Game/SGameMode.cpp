// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameMode.h"
#include "Controller/SPlayerController.h"
#include "Character/SPlayerPawn.h"
#include "Game/SPlayerState.h"
#include "Game/SGameState.h"

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

	ASGameState* SGameState = GetGameState<ASGameState>();
	if (false == SGameState) return;

	// 매칭 상태가 Waiting이 아닌 상태에서 접속했다면 바로 죽이기(튕기게 만들기)
	if (SGameState->MatchState != EMatchState::Waiting)
	{
		NewPlayer->SetLifeSpan(0.1f);
		return;
	}

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

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1.0초마다 한 번씩 OnMainTimerElapsed 호출
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);
	RemainWaitingTimeForPlaying = WaitingTime;
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

void ASGameMode::OnControllerDead(ASPlayerController* InDeadController)
{
	// 해당 컨트롤러가 유효한지 확인, 유효하지 않다면 실행되지 않게 return
	if (false == IsValid(InDeadController) || INDEX_NONE == AlivePlayerControllers.Find(InDeadController)) return;

	// Alive 빼고 Dead 추가
	AlivePlayerControllers.Remove(InDeadController);
	DeadPlayerControllers.Add(InDeadController);

}

void ASGameMode::OnMainTimerElapsed()
{
	ASGameState* SGameState = GetGameState<ASGameState>();
	if (false == IsValid(SGameState)) return;

	switch (SGameState->MatchState)
	{
	case EMatchState::None:
		break;
	// 다른 플레이어를 기다리는 중일 때
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		// 플레이어 수가 채워지지 않은, 충족되지 않으면 기다리고 있다는 것을 알려주기 위함
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another players for playing."));

			// 최소인원이 안 된다면 대기 시간 초기화
			RemainWaitingTimeForPlaying = WaitingTime; 
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

			// 접속 되는 만큼 감소
			--RemainWaitingTimeForPlaying;
		}

		// 0이 되면 실행
		if (0 == RemainWaitingTimeForPlaying)
		{
			NotificationString = FString::Printf(TEXT(""));

			SGameState->MatchState = EMatchState::Playing;
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Playing:
	{
		if (true == IsValid(SGameState))
		{
			SGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();

			FString NotificationString = FString::Printf(TEXT("%d / %d"), SGameState->AlivePlayerControllerCount, SGameState->AlivePlayerControllerCount + DeadPlayerControllers.Num());

			NotifyToAllPlayer(NotificationString);

			// 최후 1인이면 Ending
			if (1 >= SGameState->AlivePlayerControllerCount)
			{
				SGameState->MatchState = EMatchState::Ending;
			}
		}

		break;
	}
	case EMatchState::Ending:
		break;
	case EMatchState::End:
		break;
	default:
		break;
	}

}

// 플레이어 컨트롤러 마다 노피타이 적용하기 위함
void ASGameMode::NotifyToAllPlayer(const FString& NotificationString)
{
	// 살았어도 죽었어도 동일하게 처리해주기 위함
	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->NotificationText = FText::FromString(NotificationString);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->NotificationText = FText::FromString(NotificationString);
	}

}
