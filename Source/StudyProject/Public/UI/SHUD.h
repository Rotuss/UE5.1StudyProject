// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHUD.generated.h"

/**
 * 
 */
// ��ȭ�� ������ �� ��
UCLASS()
class STUDYPROJECT_API USHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindStatComponent(class USStatComponent* InStatComponent);

	void BindPlayerState(class ASPlayerState* InPlayerState);

protected:
	UFUNCTION()
	void OnKillCountChanged(int32 InOldKillCount, int32 InNewKillCount);

protected:
	// BindWidget Ű���带 ���ؼ� �ϵ��ڵ����� �Ӽ��� ������ ���ε� ���� �ʾƵ� ��
	// ��� �Ӽ��� �̸��� ���� �������Ʈ �� ������ �̸��� ���̾� ��

	// �� ų�ߴ��� Text�� ǥ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> KillCountText;

	// �÷��̾� �̸��� Text�� ǥ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	// PC�� HP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class USW_HPBar> HPBar;

	// HPBar ������ ��� USStatComponent�� �����Ƿ� ��������
	TWeakObjectPtr<class USStatComponent> StatComponent;

	// KillCountText�� PlayerNameText�� ��� ASPlayerState�� �����Ƿ� ��������
	TWeakObjectPtr<class ASPlayerState> PlayerState;

};
