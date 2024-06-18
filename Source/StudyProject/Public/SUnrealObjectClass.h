// Fill out your copyright notice in the Description page of Project Settings.

// 순환 헤더 파일을 막기 위함
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"				// UObject 파일을 상속 받기 위함
#include "SUnrealObjectClass.generated.h"		// UHT, 매크로를 변환 정리하기 위함

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USUnrealObjectClass : public UObject
{
public:
	GENERATED_BODY()

public:
	USUnrealObjectClass();

	UFUNCTION()
	void HelloUnreal();

	const FString& GetName() const { return Name; }

protected:
	UPROPERTY()
	FString Name;
};
