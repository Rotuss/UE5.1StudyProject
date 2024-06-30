// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SCharacter.h"
#include "InputActionValue.h"
#include "Game/SPlayerState.h"
#include "SViewCharacter.generated.h"

// 뷰모드를 위한 구조체
// uint8로 크기 제한
UENUM(BlueprintType)
enum class EViewMode : uint8
{
	None,
	BackView,
	QuarterView,
	TPSView,
	End,

};
/**
 * 
 */
UCLASS()
class STUDYPROJECT_API ASViewCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASViewCharacter();

public:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Replicated 사용하기 위한 작업
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetViewMode(EViewMode InViewMode);

	void SetMeshMaterial(const EPlayerTeam& InPlayerTeam);

	float GetForwardInputValue() const { return ForwardInputValue; }
	float GetRightInputValue() const { return RightInputValue; }

	float GetCurrentAimPitch() const { return CurrentAimPitch; }
	float GetCurrentAimYaw() const { return CurrentAimYaw; }

	UParticleSystemComponent* GetParticleSystem() const { return ParticleSystemComponent; }

	// PlayerState 쪽으로 이동
	/*void AddCurrentKillCount(int32 InCurrentKillCount);
	void SetMaxKillCount(int32 InMaxKillCount) { MaxKillCount = InMaxKillCount; }

	int32 GetMaxKillCount() const { return MaxKillCount; }
	int32 GetCurrentKillCount() const { return CurrentKillCount; }*/

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/* Server RPC의 실행 흐름
	 1. 특정 작업 실행 조건 시작(키를 누른다거나 이벤트 작용이 된다거나 등)
	 2. 해당 조건 호출 실행한 Owning Client의 특정 함수 호출
	 3. 서버로 보내는 패킷에 Server RPC 함수의 정보도 Serialize해서 보냄
	 4. 서버에서는 패킷을 Deserialize 후 Owning Client의 특정 Server RPC 함수 수행
	 5. 서버에서 특정 작업(지금 코드에서는 BP_LandMine 스폰)
	 5-1. 예)BP_LandMine은 Actor Replicates 속성이 true이므로 클라들에게 스폰 정보 복제
	*/
	// Server: 서버에서 수행되게하는 키워드
	// Reliable: 기본 값은 Unreliable(한 번쯤 씹혀도 되는것), 물리 작용 혹은 스폰과 같은 경우에는 Reliable 사용
	// WithValidation: 검증 및 변조 작업
	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnLandMine_Server();
	
	UFUNCTION(Server, Reliable)
	void SpawnWeaponInstance_Server();

	UFUNCTION(Server, Reliable)
	void DestroyWeaponInstance_Server();

	// 재정의
	virtual void OnRep_WeaponInstance() override;

	// Unreliable를 한 이유: 1 ~ 2번 정도는 씹혀도 되기 때문
	UFUNCTION(Server, Unreliable) 
	void UpdateInputValue_Server(const float& InForwardInputValue, const float& InRightInputValue);

	UFUNCTION(Server, Unreliable)
	void UpdateAimValue_Server(const float& InAimPitchValue, const float& InAimYawValue);

	UFUNCTION(Server, Unreliable)
	void PlayAttackMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackMontage_NetMulticast();

	// 라인 트레이싱 관련은 Owner에서 대미지 적용은 Server에서
	UFUNCTION(Server, Reliable)
	void ApplyDamageAndDrawLine_Server(FHitResult HitResult);

	UFUNCTION(NetMulticast, Reliable)
	void DrawLine_NetMulticast(const FVector& InDrawStart, const FVector& InDrawEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void PlayRagdoll_NetMulticast();

	UFUNCTION()
	void OnHittedRagdollRestoreTimerElapsed();
	
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void ChangeView(const FInputActionValue& InValue);

	void InputQuickSlot01(const FInputActionValue& InValue);

	void InputQuickSlot02(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);

	void Menu(const FInputActionValue& InValue);

	void StartIronSight(const FInputActionValue& InValue);

	void EndIronSight(const FInputActionValue& InValue);

	void ToggleTrigger(const FInputActionValue& InValue);

	void StartFire(const FInputActionValue& InValue);

	void StopFire(const FInputActionValue& InValue);

	void SpawnLandMine(const FInputActionValue& InValue);

	void TryFire();

protected:
	// 추가한 모듈에서 어떤 색상의 마테리얼을 사용할지 지정하기 위함
	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath = FSoftObjectPath();

	// 로딩할 때 쓰기위한 핸들
	// 핸들 값을 알아야지 메시로드 이후 해당 정보를 취할 수 있음
	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	// 무기 장착시 몽타주 기억
	UPROPERTY()
	TObjectPtr<UAnimMontage> UnequipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<UCameraShakeBase> FireShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASViewCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<AActor> LandMineClass;

	// 무기 장착시 애니메이션 레이어 기억
	UPROPERTY()
	TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ForwardInputValue;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float RightInputValue;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float CurrentAimPitch = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float CurrentAimYaw = 0.0f;

	// 분당 탄환을 몇 번 발사할 것인가
	// 숫자가 클수록 빠르게 연사
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float FirePerMinute = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

	// PlayerState 쪽으로 이동
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 CurrentKillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 MaxKillCount = 99;*/

	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	// 피격 랙돌을 위한 타이머 핸들
	FTimerHandle HittedRagdollRestoreTimer;

	FTimerHandle BetweenShotsTimer;

	// UPROPERTY() 매크로를 사용하지 않으므로 초기화에 유념해야함
	EViewMode CurrentViewMode = EViewMode::None;

	FVector DirectionToMove = FVector::ZeroVector;
	FRotator DestArmRotation = FRotator::ZeroRotator;

	float DestArmLength = 0.0f;
	float ArmLengthChangeSpeed = 3.0f;
	float ArmRotationChangeSpeed = 10.0f;

	// Zoom 작업
	float TargetFOV = 70.0f;
	float CurrentFOV = 70.0f;

	// 연사 작업
	float TimeBetweenFire;
	bool bIsTriggerToggle = false;

	// 현재 값과 달라졌을 때 Replicated 하기 위한 작업
	float PreviousForwardInputValue = 0.0f;
	float PreviousRightInputValue = 0.0f;
	float PreviousAimPitch = 0.0f;
	float PreviousAimYaw = 0.0f;

	// 랙돌 보간 작업
	float TargetRagDollBlendWeight = 0.0f;
	float CurrentRagDollBlendWeight = 0.0f;
	bool bIsNowRagdollBlending = false;

};
