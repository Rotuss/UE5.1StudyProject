// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SUIPlayerController_Loading.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void ASUIPlayerController_Loading::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (true == IsValid(GameMode))
	{
		// ParseOption�Լ��� �̿��Ͽ� Ű���� FString(TEXT("NextLevel"))�� ���� �Ľ̵� ���� ���� �� ����
		FString NextLevelString = UGameplayStatics::ParseOption(GameMode->OptionsString, FString(TEXT("NextLevel")));
		UGameplayStatics::OpenLevel(GameMode, *NextLevelString, false);
	}

}
