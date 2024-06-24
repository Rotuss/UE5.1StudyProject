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
	// 초기화 및 바인드 작업
	virtual void NativeOnInitialized() override;

	// 정리 작업
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

	// 이 메시가 필요해 이 마테리얼이 필요해 하고 로딩해줘 할 때 취하기 위한 핸들
	TArray<TSharedPtr<struct FStreamableHandle>> StreamableHandles;

	// 로딩이 완료되고 마테리얼을 저장할 배열
	TArray<TSoftObjectPtr<UMaterial>> LoadedMaterialInstanceAssets;

	// 선택한 팀
	uint8 SelectedTeam = 1;
};
