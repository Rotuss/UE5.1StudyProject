// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentHPChangeDelegate, float, InOldCurrentHP, float, InNewCurrentHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHPChangeDelegate, float, InOldMaxHP, float, InNewMaxHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYPROJECT_API USStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USStatComponent();

public:
	void SetMaxHP(float InMaxHP);

	void SetCurrentHP(float InCurrentHP);

	//void SetDead(bool InDead);

	float GetMaxHP() const { return MaxHP; }

	float GetCurrentHP() const { return CurrentHP; }

	//bool IsDead() const { return bIsDead; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:
	UFUNCTION(NetMulticast, Reliable)
	void OnCurrentHPChanged_NetMulticast(float InOldCurrentHP, float InNewCurrentHP);

	/*UFUNCTION()
	void OnRep_IsDead();*/

public:
	// ��������Ʈ�� ���ε��� ���� ����� ������ UI ������Ʈ �۾��� ���� �κ�
	// UI���� Tick�� ���� ����ؼ� Get Set�Լ��� ���� ó���� �� ������ �׷� ��� ���ϰ� �ɸ� �� ����
	// �� �ʿ��� ���� ����� ���� ������Ʈ �۾��� �̷��� �� �ְ� ��������Ʈ �̿�
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHPDelegate;

	FOnCurrentHPChangeDelegate OnCurrentHPChangedDelegate;

	FOnMaxHPChangeDelegate OnMaxHPChangedDelegate;

private:
	// ���� �ν��Ͻ��� ���� �ִ� ������ ���������̺��� ���� �ֱ� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	TObjectPtr<class USGameInstance> GameInstance;

	// ü���� ���� �ִ� ������ �÷��̾� ĳ���� �Ӹ� �ƴ϶� ���͵� ü���� �����ϰ� �ֱ� ����, �� ������ �۾��� ���ϱ� ����
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	float MaxHP;

	// Transient��? �ܼ��� ���ؼ� ������ �� �ϴ� �Ͱ� ����. �� Transient�� ���� �ִ� �Ӽ��� ��ü �ܿ��� ��𼭵� ������ ���ϰ�
	// ������ CDO�� �ִ� ���� �⺻ ������ ����
	// �޸��� ���鿡�� ���� �ֹ߼��� �����Ƿ�!
	// Transient�� ��� ��ü���� �ٸ��� ���ư��� ��(����ü�°� ����, ��κ� Current�� ���̸� ���ٰ� �����ϸ� ��)
	// �ø��������� ���ܽ�Ŵ
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	float CurrentHP;

	/*UPROPERTY(ReplicatedUsing = OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	bool bIsDead;*/

};
