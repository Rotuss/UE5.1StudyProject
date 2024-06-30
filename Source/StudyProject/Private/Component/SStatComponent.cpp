// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SStatComponent.h"
#include "Game/SGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
//#include "Character/SCharacter.h"

// �̺�Ʈ�� ���� ��ɿ��� ������ ������ ����Ǹ� �ش� ������Ʈ�� ������Ʈ
// ���Ŀ� ���� ������Ʈ �ʿ��� ó���� �޾� ���� ������Ʈ�� ������Ʈ

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
	// ��������Ʈ ���ε� Ȯ��
	if (true == OnMaxHPChangedDelegate.IsBound())
	{
		OnMaxHPChangedDelegate.Broadcast(MaxHP, InMaxHP);
	}

	MaxHP = FMath::Clamp<float>(InMaxHP, 0.0f, 9999);

}

void USStatComponent::SetCurrentHP(float InCurrentHP)
{
	// ��������Ʈ ���ε� Ȯ��
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

	// NetMulticast�� ��� Server������ ��ȿ�ѵ� ��� �Ǵ� ��?
	// => SetCurrentHP�� ȣ���ϴ� �κ��� Character���� TakeDamage �κ�
	// TakeDamage�� �������� ȣ��Ǵ� �Լ���? ���� SetCurrentHP�� �������� ȣ��ǰ� �ִ� ��
	// ���� NetMulticast�� �������� �����
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
	
	// ���� �ν��Ͻ� ��������
	GameInstance = Cast<USGameInstance>(GetWorld()->GetGameInstance());
	if (true == IsValid(GameInstance))
	{
		// ������ ���̺� ��������
		if (nullptr != GameInstance->GetCharacterStatDataTable() || nullptr != GameInstance->GetCharacterStatDataTableRow(1))
		{
			// ����
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

