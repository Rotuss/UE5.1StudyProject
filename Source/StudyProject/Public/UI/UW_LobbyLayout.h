// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLayout.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API UUW_LobbyLayout : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// �ʱ�ȭ �� ���ε� �۾�
	virtual void NativeOnInitialized() override;

	// ���� �۾�
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnBlackTeamButtonClicked();

	UFUNCTION()
	void OnWhiteTeamButtonClicked();

	UFUNCTION()
	void OnSubmitButtonClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> BlackTeamButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> WhiteTeamButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UEditableText> EditPlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<class UButton> SubmitButton;

	TWeakObjectPtr<USkeletalMeshComponent> CurrentSkeletalMeshComponent;

	// �� �޽ð� �ʿ��� �� ���׸����� �ʿ��� �ϰ� �ε����� �� �� ���ϱ� ���� �ڵ�
	TArray<TSharedPtr<struct FStreamableHandle>> StreamableHandles;

	// �ε��� �Ϸ�ǰ� ���׸����� ������ �迭
	TArray<TSoftObjectPtr<UMaterial>> LoadedMaterialInstanceAssets;

	// ������ ��
	uint8 SelectedTeam = 1;
};
