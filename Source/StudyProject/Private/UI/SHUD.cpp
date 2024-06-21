// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SHUD.h"
#include "Components/TextBlock.h"
#include "Component/SStatComponent.h"
#include "Game/SGameInstance.h"
#include "Game/SPlayerState.h"
#include "UI/SW_HPBar.h"

// HPBar를 업데이트 해주기 위해 StatComponent를 바인드 하는 작업
void USHUD::BindStatComponent(USStatComponent* InStatComponent)
{
	if (true == IsValid(InStatComponent))
	{
		StatComponent = InStatComponent;
		// StatComponent를 가져와서 HPBar랑 OnCurrentHPChange, OnMaxHPChange 바인드
		// HP 관련 수치가 변경 될 때마다 호출해서 변경할 수 있게 브로드캐스트되면 호출 될 수 있게!
		StatComponent->OnCurrentHPChangedDelegate.AddDynamic(HPBar, &USW_HPBar::OnCurrentHPChange);
		StatComponent->OnMaxHPChangedDelegate.AddDynamic(HPBar, &USW_HPBar::OnMaxHPChange);

		// PlayerController::BeginPlay()가 SStatComponent::BeginPlay()보다 먼저 호출되기에 아래와 같이 초기화
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
