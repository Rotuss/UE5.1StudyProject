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

// ���� ���� �ʱ�ȭ �κ� ȣ��
void ASGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        InitGame()"));
	Super::InitGame(MapName, Options, ErrorMessage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        InitGame()"));
}

// state�� ��������Ƽ�� ������ �� ��Ȯ����
// ����� �̷��� �����Ѵ� �������� �����ϸ� ��
// ���Ӹ���� ���, ���� ���� �� ������ ����, �ߴ�
// ���ӽ�����Ʈ�� ���� �÷��̾ �󸶳� ���Ҵ��� ��ü���� ���� ��Ȳ ��
void ASGameMode::InitGameState()
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        InitGameState()"));
	Super::InitGameState();
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        InitGameState()"));
}

// ������Ʈ���� ��� �ʱ�ȭ �� �� �۾�
void ASGameMode::PostInitializeComponents()
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PostInitializeComponents()"));
	Super::PostInitializeComponents();
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PostInitializeComponents()"));
}

// �α��� ��
void ASGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMassage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PreLogin()"));
	Super::PreLogin(Options, Address, UniqueId, ErrorMassage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PreLogin()"));
}

// �α��� ��
// UPlayer* NewPlayer ���� �ο��� ����
APlayerController* ASGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        Login()"));
	APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        Login()"));

	return PlayerController;
}

// �α��� ��
void ASGameMode::PostLogin(APlayerController* NewPlayer)
{
	//UE_LOG(LogTemp, Error, TEXT("         Start ASGameMode::        PostLogin(ASPlayerController)"));
	Super::PostLogin(NewPlayer);
	//UE_LOG(LogTemp, Error, TEXT("         End   ASGameMode::        PostLogin(ASPlayerController)"));

	ASGameState* SGameState = GetGameState<ASGameState>();
	if (false == SGameState) return;

	// ��Ī ���°� Waiting�� �ƴ� ���¿��� �����ߴٸ� �ٷ� ���̱�(ƨ��� �����)
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
		// �α��� �� ���� ���� ǥ�� �˸�
		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}

}

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1.0�ʸ��� �� ���� OnMainTimerElapsed ȣ��
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
	// �ش� ��Ʈ�ѷ��� ��ȿ���� Ȯ��, ��ȿ���� �ʴٸ� ������� �ʰ� return
	if (false == IsValid(InDeadController) || INDEX_NONE == AlivePlayerControllers.Find(InDeadController)) return;

	// Alive ���� Dead �߰�
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
	// �ٸ� �÷��̾ ��ٸ��� ���� ��
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		// �÷��̾� ���� ä������ ����, �������� ������ ��ٸ��� �ִٴ� ���� �˷��ֱ� ����
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another players for playing."));

			// �ּ��ο��� �� �ȴٸ� ��� �ð� �ʱ�ȭ
			RemainWaitingTimeForPlaying = WaitingTime; 
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

			// ���� �Ǵ� ��ŭ ����
			--RemainWaitingTimeForPlaying;
		}

		// 0�� �Ǹ� ����
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

			// ���� 1���̸� Ending
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

// �÷��̾� ��Ʈ�ѷ� ���� ����Ÿ�� �����ϱ� ����
void ASGameMode::NotifyToAllPlayer(const FString& NotificationString)
{
	// ��Ҿ �׾�� �����ϰ� ó�����ֱ� ����
	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->NotificationText = FText::FromString(NotificationString);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->NotificationText = FText::FromString(NotificationString);
	}

}
