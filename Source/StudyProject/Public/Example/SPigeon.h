// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFlyable.h"
#include "SPigeon.generated.h"

USTRUCT()
struct FPigeonData
{
	GENERATED_BODY()

public:
	FPigeonData() {}
	// 언리얼 코딩 컨벤션에 따르면 입력 전용 매개변수명에 In-, 출력 전용 매개변수명에 Out-
	FPigeonData(const FString& InName, int32 InID)
		: Name(InName)
		, ID(InID)
	{}
	
	// 시리얼라이즈 작업을 위한 연산자 재정의
	// 해당 작업을 해줘야 시리얼라이제이션이 제대로 작동
	friend FArchive& operator<<(FArchive& InArchive, FPigeonData& InPigeonData)
	{
		InArchive << InPigeonData.Name;
		InArchive << InPigeonData.ID;

		return InArchive;
	}

public:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 ID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPigeonFlying, const FString&, InName, const int32, InID);

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USPigeon
	: public UObject
	, public ISFlyable
{
	GENERATED_BODY()

public:
	USPigeon();

	virtual void Fly() override;

	const FString& GetPigeonName() const { return Name; }
	void SetPigeonName(const FString& InName) { Name = InName; }

	const int32& GetPigeonID() const { return ID; }
	void SetPigeonID(const int32& InID) { ID = InID; }

	virtual void Serialize(FArchive& Ar) override;

public:
	FOnPigeonFlying OnPigeonFlying;

private:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 ID = 0;
};