// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SHUD.h"
#include "Components/TextBlock.h"
#include "Component/SStatComponent.h"
#include "Game/SGameInstance.h"
#include "Game/SPlayerState.h"
#include "UI/SW_HPBar.h"

// HPBar�� ������Ʈ ���ֱ� ���� StatComponent�� ���ε� �ϴ� �۾�
void USHUD::BindStatComponent(USStatComponent* InStatComponent)
{
	if (true == IsValid(InStatComponent))
	{
		StatComponent = InStatComponent;
		// StatComponent�� �����ͼ� HPBar�� OnCurrentHPChange, OnMaxHPChange ���ε�
		// HP ���� ��ġ�� ���� �� ������ ȣ���ؼ� ������ �� �ְ� ��ε�ĳ��Ʈ�Ǹ� ȣ�� �� �� �ְ�!
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBar, &USW_HPBar::OnCurrentHPChange);
		StatComponent->OnMaxHPChangedDelegate.AddDynamic(HPBar, &USW_HPBar::OnMaxHPChange);

		// PlayerController::BeginPlay()�� SStatComponent::BeginPlay()���� ���� ȣ��Ǳ⿡ �Ʒ��� ���� �ʱ�ȭ
		USGameInstance* GameInstance = Cast<USGameInstance>(GetWorld()->GetGameInstance());
		if (true == IsValid(GameInstance))
		{
			if (nullptr != GameInstance->GetCharacterStatDataTable() || nullptr != GameInstance->GetCharacterStatDataTableRow(1))
			{
				float MaxHP = GameInstance->GetCharacterStatDataTableRow(1)->MaxHP;
				HPBar->SetMaxHP(MaxHP);
				HPBar->InitializeHPBarWidget(StatComponent.Get());
			}
		}
	}

}

void USHUD::BindPlayerState(ASPlayerState* InPlayerState)
{
	if (true == IsValid(InPlayerState))
	{
		PlayerState = InPlayerState;
		PlayerState->OnCurrentKillCountChangedDelegate.AddDynamic(this, &ThisClass::OnKillCountChanged);

		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
		OnKillCountChanged(0, PlayerState->GetCurrentKillCount());
	}

}

void USHUD::OnKillCountChanged(int32 InOldKillCount, int32 InNewKillCount)
{
	FString KillCountString = FString::Printf(TEXT("%d"), InNewKillCount);
	KillCountText->SetText(FText::FromString(KillCountString));

}
