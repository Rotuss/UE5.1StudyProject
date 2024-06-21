// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHUD.generated.h"

/**
 * 
 */
// 도화지 역할이 될 것
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
	// BindWidget 키워드를 통해서 하드코딩으로 속성과 위젯을 바인드 하지 않아도 됨
	// 대신 속성의 이름과 위젯 블루프린트 속 위젯의 이름이 같이야 함

	// 몇 킬했는지 Text로 표현
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> KillCountText;

	// 플레이어 이름을 Text로 표현
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	// PC용 HP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class USW_HPBar> HPBar;

	// HPBar 상태의 경우 USStatComponent에 있으므로 가져오기
	TWeakObjectPtr<class USStatComponent> StatComponent;

	// KillCountText와 PlayerNameText의 경우 ASPlayerState에 있으므로 가져오기
	TWeakObjectPtr<class ASPlayerState> PlayerState;

};
