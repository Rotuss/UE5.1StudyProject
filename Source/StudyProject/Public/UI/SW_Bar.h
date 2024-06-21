// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/StudyWidget.h"
#include "SW_Bar.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USW_Bar : public UStudyWidget
{
	GENERATED_BODY()
	
public:
	// 만약 기본 생성자를 못쓰기 때문에 Widget 클래스의 생성자에 로직을 작성한다면 꼭 이 생성자를 선언/정의해줘야 함
	USW_Bar(const FObjectInitializer& ObjectInitializer);

public:
	void SetMaxFigure(float InMaxFigure);

protected:
	virtual void NativeConstruct() override;

protected:
	// 프로그레스 바
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
	TObjectPtr<class UProgressBar> Bar;

	// 최대값(피통)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
	float MaxFigure;

};
