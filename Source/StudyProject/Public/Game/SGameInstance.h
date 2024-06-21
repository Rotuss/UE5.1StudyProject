// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"		// UGameInstance�� ��� �ޱ� ����
#include "Engine/DataTable.h"
#include "SGameInstance.generated.h"

class USPigeon;

USTRUCT(BlueprintType)
struct FSStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSStatTableRow()
	{

	}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxKillCount;

};

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USGameInstance();

public:
	// �̺�Ʈ �Լ�, �����Ϳ��� �÷��� ��ư�� ������ �� ȣ��
	virtual void Init() override;

	// �̺�Ʈ �Լ�, �����Ϳ��� �÷��̸� ������ �� ȣ��
	virtual void Shutdown() override;

	const UDataTable* GetCharacterStatDataTable() const { return CharacterStatDataTable; }

	FSStatTableRow* GetCharacterStatDataTableRow(int32 InLevel);

	// ���̳��� ��������Ʈ�̱� ������ ���������� ����� �� �ְ� UFUNCTION() ��ũ�� ���
	/*UFUNCTION()
	void HandlePigeonFlying(const FString& InName, const int32 InID);*/

protected:
	/*UPROPERTY()
	FString Name;

	UPROPERTY()
	TObjectPtr<USPigeon> SpawnedPigeon;*/

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UDataTable> CharacterStatDataTable;
};
