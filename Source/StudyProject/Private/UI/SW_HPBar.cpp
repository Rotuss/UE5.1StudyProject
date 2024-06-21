 // Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SW_HPBar.h"
#include "Components/ProgressBar.h"
#include "Character/SCharacter.h"
#include "Component/SStatComponent.h"

void USW_HPBar::OnMaxHPChange(float InOldMaxHP, float InNewMaxHP)
{
	SetMaxFigure(InNewMaxHP);

	OnCurrentHPChange(0.0f, InNewMaxHP);
}

void USW_HPBar::OnCurrentHPChange(float InOldHP, float InNewHP)
{
	if (true == IsValid(Bar))
	{
		if (KINDA_SMALL_NUMBER < MaxFigure)	Bar->SetPercent(InNewHP / MaxFigure);
		else Bar->SetPercent(0.0f);
	}
}

void USW_HPBar::SetMaxHP(float InMaxHP)
{
	SetMaxFigure(InMaxHP);

}

void USW_HPBar::InitializeHPBarWidget(USStatComponent* NewStatComponent)
{
	if (true == IsValid(NewStatComponent))
	{
		OnCurrentHPChange(0.0f, NewStatComponent->GetCurrentHP());
	}
}

void USW_HPBar::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯을 갖고 있는 액터를 ASCharacter로 캐스팅해서 성공시 SetWidget 작업
	ASCharacter* OwningCharacter = Cast<ASCharacter>(OwningActor);
	if (true == IsValid(OwningCharacter))
	{
		OwningCharacter->SetWidget(this);
	}
}
