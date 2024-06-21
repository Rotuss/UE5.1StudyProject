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
	// ���� �⺻ �����ڸ� ������ ������ Widget Ŭ������ �����ڿ� ������ �ۼ��Ѵٸ� �� �� �����ڸ� ����/��������� ��
	USW_Bar(const FObjectInitializer& ObjectInitializer);

public:
	void SetMaxFigure(float InMaxFigure);

protected:
	virtual void NativeConstruct() override;

protected:
	// ���α׷��� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
	TObjectPtr<class UProgressBar> Bar;

	// �ִ밪(����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
	float MaxFigure;

};
