// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SViewCharacterSettings.generated.h"

/**
 * 
 */
 // 1. 언리얼 엔진의 초기화 단계에서 Config 폴더에 위치한 DefaultPlayerCharacterMeshPaths.ini 파일을 읽어들임
UCLASS(config = PlayerCharacterMeshMaterialPaths) 
class STUDYPROJECTSETTINGS_API USViewCharacterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// 2. 읽어들인 PlayerCharacterMeshMaterialPaths.ini 파일의 내용으로 해당 멤버의 기본값이 초기화됨
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerCharacterMeshMaterialPaths;

};
