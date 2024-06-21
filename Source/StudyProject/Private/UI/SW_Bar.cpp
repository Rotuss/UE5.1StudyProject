// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SW_Bar.h"
#include "Components/ProgressBar.h"

USW_Bar::USW_Bar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USW_Bar::SetMaxFigure(float InMaxFigure)
{
	if (KINDA_SMALL_NUMBER > InMaxFigure)
	{
		MaxFigure = 0.0f;

		return;
	}

	MaxFigure = InMaxFigure;
}

void USW_Bar::NativeConstruct()
{
	Super::NativeConstruct();

	// �ϵ��ڵ����� ������ �Ӽ��� ���ε��ϴ� ���
	Bar = Cast<UProgressBar>(GetWidgetFromName("BarWidget"));
	checkf(true == IsValid(Bar), TEXT("Invalid Bar"));
}
