// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPlayerState.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/SViewCharacter.h"

ASPlayerState::ASPlayerState()
{
}

// 만약 경험치 개념을 구현한다면 이 함수에서 GameInstance를 통한 초기화 필요
void ASPlayerState::InitPlayerState()
{
	SetPlayerName(TEXT("Player"));

	CurrentKillCount = 0;
	MaxKillCount = 99;

	// 로비 레이아웃에서 저장한 것을 불러들이기
	const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	const FString SavedFileName(TEXT("PlayerInfo.txt"));
	FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
	FPaths::MakeStandardFilename(AbsoluteFilePath);

	// JSON 읽기
	FString PlayerInfoJsonString;
	FFileHelper::LoadFileToString(PlayerInfoJsonString, *AbsoluteFilePath);
	TSharedRef<TJsonReader<TCHAR>> JsonReaderArchive = TJsonReaderFactory<TCHAR>::Create(PlayerInfoJsonString);

	// JSON Object 만들기
	TSharedPtr<FJsonObject> PlayerInfoJsonObject = nullptr;
	// 디시리얼라이즈(바이트 -> 오브젝트)
	if (FJsonSerializer::Deserialize(JsonReaderArchive, PlayerInfoJsonObject) == true)
	{
		// 이름 설정
		FString PlayerNameString = PlayerInfoJsonObject->GetStringField(TEXT("playername"));
		SetPlayerName(PlayerNameString);

		// 팀 값 설정
		uint8 PlayerTeamNumber = PlayerInfoJsonObject->GetIntegerField(TEXT("team"));
		PlayerTeam = static_cast<EPlayerTeam>(PlayerTeamNumber);
		ASViewCharacter* PlayerCharacter = Cast<ASViewCharacter>(GetPawn());
		if (true == IsValid(PlayerCharacter))
		{
			// 팀에 따른 머테리얼 세팅
			PlayerCharacter->SetMeshMaterial(PlayerTeam);
		}
	}
}

void ASPlayerState::AddCurrentKillCount(int32 InCurrentKillCount)
{
	OnCurrentKillCountChangedDelegate.Broadcast(CurrentKillCount, CurrentKillCount + InCurrentKillCount);

	// 킬 카운트 증가
	CurrentKillCount = FMath::Clamp(CurrentKillCount + InCurrentKillCount, 0, MaxKillCount);

	ASViewCharacter* PlayerCharacter = Cast<ASViewCharacter>(GetPawn());
	checkf(true == IsValid(PlayerCharacter), TEXT("Invalid PlayerCharacter"));
	PlayerCharacter->GetParticleSystem()->Activate(true);
}
