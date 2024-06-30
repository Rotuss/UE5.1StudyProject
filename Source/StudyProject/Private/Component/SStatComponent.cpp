// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SStatComponent.h"
#include "Game/SGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
//#include "Character/SCharacter.h"

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
	//bIsDead = false;

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

	// NetMulticast의 경우 Server에서만 유효한데 어떻게 되는 것?
	// => SetCurrentHP을 호출하는 부분은 Character에서 TakeDamage 부분
	// TakeDamage는 서버에서 호출되는 함수다? 따라서 SetCurrentHP도 서버에서 호출되고 있는 것
	// 따라서 NetMulticast가 문제없이 수행됨
	OnCurrentHPChanged_NetMulticast(CurrentHP, CurrentHP);

}

//void USStatComponent::SetDead(bool InDead)
//{
//	bIsDead = InDead;
//
//}

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

void USStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHP);
	DOREPLIFETIME(ThisClass, CurrentHP);
	//DOREPLIFETIME(ThisClass, bIsDead);

}

//void USStatComponent::OnRep_IsDead()
//{
//	if (true == bIsDead)
//	{
//		// Ragdoll
//		ASCharacter* Temp = Cast<ASCharacter>(GetOwner());
//		Temp->GetMesh()->SetSimulatePhysics(true);
//	}
//}

void USStatComponent::OnCurrentHPChanged_NetMulticast_Implementation(float InOldCurrentHP, float InNewCurrentHP)
{
	if (true == OnCurrentHPChangedDelegate.IsBound())
	{
		OnCurrentHPChangedDelegate.Broadcast(InOldCurrentHP, InNewCurrentHP);
	}

	if (KINDA_SMALL_NUMBER > InNewCurrentHP)
	{
		OnOutOfCurrentHPDelegate.Broadcast();
	}

}

