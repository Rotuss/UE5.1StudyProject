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
	// 델리게이트에 바인드해 값이 변경될 때마다 UI 업데이트 작업을 위한 부분
	// UI에서 Tick을 통해 계속해서 Get Set함수를 통해 처리할 수 있지만 그럴 경우 부하가 걸릴 수 있음
	// 즉 필요할 때만 변경될 때만 업데이트 작업이 이뤄질 수 있게 델리게이트 이용
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHPDelegate;

	FOnCurrentHPChangeDelegate OnCurrentHPChangedDelegate;

	FOnMaxHPChangeDelegate OnMaxHPChangedDelegate;

private:
	// 게임 인스턴스를 갖고 있는 이유는 데이터테이블을 갖고 있기 때문
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	TObjectPtr<class USGameInstance> GameInstance;

	// 체력을 갖고 있는 이유는 플레이어 캐릭터 뿐만 아니라 몬스터도 체력을 보유하고 있기 때문, 즉 동일한 작업을 요하기 때문
	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	float MaxHP;

	// Transient란? 단순히 말해서 저장을 안 하는 것과 같음. 즉 Transient을 갖고 있는 속성은 개체 외에도 어디서든 저장을 안하고
	// 무조건 CDO에 있는 값이 기본 값으로 설정
	// 메모리적 측면에서 유용 휘발성이 있으므로!
	// Transient의 경우 개체마다 다르게 돌아가는 것(현재체력과 같은, 대부분 Current를 붙이면 쓴다고 생각하면 됨)
	// 시리얼라이즈에서 제외시킴
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	float CurrentHP;

	/*UPROPERTY(ReplicatedUsing = OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly, Category = "USStatComponent", meta = (AllowPrivateAccess))
	bool bIsDead;*/

};
