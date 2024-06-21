// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SStatComponent.h"
#include "Game/SGameInstance.h"

// 이벤트와 같은 기능에서 액터의 스탯이 변경되면 해당 컴포넌트를 업데이트
// 이후에 위젯 컴포넌트 쪽에서 처리를 받아 위젯 컴포넌트도 업데이트

// Sets default values for this component's properties
USStatComponent::USStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	bWantsInitializeComponent = false;

}

void USStatComponent::SetMaxHP(float InMaxHP)
{
	// 델리게이트 바인드 확인
	if (true == OnMaxHPChangedDelegate.IsBound())
	{
		OnMaxHPChangedDelegate.Broadcast(MaxHP, InMaxHP);
	}

	MaxHP = FMath::Clamp<float>(InMaxHP, 0.0f, 9999);

}

void USStatComponent::SetCurrentHP(float InCurrentHP)
{
	// 델리게이트 바인드 확인
	if (true == OnCurrentHPChangedDelegate.IsBound())
	{
		OnCurrentHPChangedDelegate.Broadcast(CurrentHP, InCurrentHP);
	}

	CurrentHP = FMath::Clamp<float>(InCurrentHP, 0.0f, MaxHP);

	if (KINDA_SMALL_NUMBER > CurrentHP)
	{
		OnOutOfCurrentHPDelegate.Broadcast();
		CurrentHP = 0.0f;
	}

}


// Called when the game starts
void USStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	// 게임 인스턴스 가져오기
	GameInstance = Cast<USGameInstance>(GetWorld()->GetGameInstance());
	if (true == IsValid(GameInstance))
	{
		// 데이터 테이블 가져오기
		if (nullptr != GameInstance->GetCharacterStatDataTable() || nullptr != GameInstance->GetCharacterStatDataTableRow(1))
		{
			// 세팅
			float NewMaxHP = GameInstance->GetCharacterStatDataTableRow(1)->MaxHP;
			SetMaxHP(NewMaxHP);
			SetCurrentHP(MaxHP);
		}
	}

}


// Called every frame
void USStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

