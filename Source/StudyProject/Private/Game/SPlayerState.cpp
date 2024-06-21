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
