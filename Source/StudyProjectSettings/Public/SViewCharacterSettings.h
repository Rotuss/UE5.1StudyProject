// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SViewCharacterSettings.generated.h"

/**
 * 
 */
 // 1. �𸮾� ������ �ʱ�ȭ �ܰ迡�� Config ������ ��ġ�� DefaultPlayerCharacterMeshPaths.ini ������ �о����
UCLASS(config = PlayerCharacterMeshMaterialPaths) 
class STUDYPROJECTSETTINGS_API USViewCharacterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// 2. �о���� PlayerCharacterMeshMaterialPaths.ini ������ �������� �ش� ����� �⺻���� �ʱ�ȭ��
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerCharacterMeshMaterialPaths;

};
