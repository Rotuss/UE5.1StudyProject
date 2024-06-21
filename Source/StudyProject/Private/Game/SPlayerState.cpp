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
