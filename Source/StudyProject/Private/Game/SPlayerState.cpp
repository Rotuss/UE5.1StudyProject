// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SPlayerState.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/SViewCharacter.h"

ASPlayerState::ASPlayerState()
{
}

// ���� ����ġ ������ �����Ѵٸ� �� �Լ����� GameInstance�� ���� �ʱ�ȭ �ʿ�
void ASPlayerState::InitPlayerState()
{
	SetPlayerName(TEXT("Player"));

	CurrentKillCount = 0;
	MaxKillCount = 99;

	// �κ� ���̾ƿ����� ������ ���� �ҷ����̱�
	const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	const FString SavedFileName(TEXT("PlayerInfo.txt"));
	FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
	FPaths::MakeStandardFilename(AbsoluteFilePath);

	// JSON �б�
	FString PlayerInfoJsonString;
	FFileHelper::LoadFileToString(PlayerInfoJsonString, *AbsoluteFilePath);
	TSharedRef<TJsonReader<TCHAR>> JsonReaderArchive = TJsonReaderFactory<TCHAR>::Create(PlayerInfoJsonString);

	// JSON Object �����
	TSharedPtr<FJsonObject> PlayerInfoJsonObject = nullptr;
	// ��ø��������(����Ʈ -> ������Ʈ)
	if (FJsonSerializer::Deserialize(JsonReaderArchive, PlayerInfoJsonObject) == true)
	{
		// �̸� ����
		FString PlayerNameString = PlayerInfoJsonObject->GetStringField(TEXT("playername"));
		SetPlayerName(PlayerNameString);

		// �� �� ����
		uint8 PlayerTeamNumber = PlayerInfoJsonObject->GetIntegerField(TEXT("team"));
		PlayerTeam = static_cast<EPlayerTeam>(PlayerTeamNumber);
		ASViewCharacter* PlayerCharacter = Cast<ASViewCharacter>(GetPawn());
		if (true == IsValid(PlayerCharacter))
		{
			// ���� ���� ���׸��� ����
			PlayerCharacter->SetMeshMaterial(PlayerTeam);
		}
	}
}

void ASPlayerState::AddCurrentKillCount(int32 InCurrentKillCount)
{
	OnCurrentKillCountChangedDelegate.Broadcast(CurrentKillCount, CurrentKillCount + InCurrentKillCount);

	// ų ī��Ʈ ����
	CurrentKillCount = FMath::Clamp(CurrentKillCount + InCurrentKillCount, 0, MaxKillCount);

	ASViewCharacter* PlayerCharacter = Cast<ASViewCharacter>(GetPawn());
	checkf(true == IsValid(PlayerCharacter), TEXT("Invalid PlayerCharacter"));
	PlayerCharacter->GetParticleSystem()->Activate(true);
}
