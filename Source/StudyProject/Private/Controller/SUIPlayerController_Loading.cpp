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
		// ParseOption함수를 이용하여 키값인 FString(TEXT("NextLevel"))을 통해 파싱된 값을 얻을 수 있음
		FString NextLevelString = UGameplayStatics::ParseOption(GameMode->OptionsString, FString(TEXT("NextLevel")));
		UGameplayStatics::OpenLevel(GameMode, *NextLevelString, false);
	}

}
