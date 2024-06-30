// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SCharacter.h"
#include "InputActionValue.h"
#include "Game/SPlayerState.h"
#include "SViewCharacter.generated.h"

// ���带 ���� ����ü
// uint8�� ũ�� ����
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

	// Replicated ����ϱ� ���� �۾�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetViewMode(EViewMode InViewMode);

	void SetMeshMaterial(const EPlayerTeam& InPlayerTeam);

	float GetForwardInputValue() const { return ForwardInputValue; }
	float GetRightInputValue() const { return RightInputValue; }

	float GetCurrentAimPitch() const { return CurrentAimPitch; }
	float GetCurrentAimYaw() const { return CurrentAimYaw; }

	UParticleSystemComponent* GetParticleSystem() const { return ParticleSystemComponent; }

	// PlayerState ������ �̵�
	/*void AddCurrentKillCount(int32 InCurrentKillCount);
	void SetMaxKillCount(int32 InMaxKillCount) { MaxKillCount = InMaxKillCount; }

	int32 GetMaxKillCount() const { return MaxKillCount; }
	int32 GetCurrentKillCount() const { return CurrentKillCount; }*/

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/* Server RPC�� ���� �帧
	 1. Ư�� �۾� ���� ���� ����(Ű�� �����ٰų� �̺�Ʈ �ۿ��� �ȴٰų� ��)
	 2. �ش� ���� ȣ�� ������ Owning Client�� Ư�� �Լ� ȣ��
	 3. ������ ������ ��Ŷ�� Server RPC �Լ��� ������ Serialize�ؼ� ����
	 4. ���������� ��Ŷ�� Deserialize �� Owning Client�� Ư�� Server RPC �Լ� ����
	 5. �������� Ư�� �۾�(���� �ڵ忡���� BP_LandMine ����)
	 5-1. ��)BP_LandMine�� Actor Replicates �Ӽ��� true�̹Ƿ� Ŭ��鿡�� ���� ���� ����
	*/
	// Server: �������� ����ǰ��ϴ� Ű����
	// Reliable: �⺻ ���� Unreliable(�� ���� ������ �Ǵ°�), ���� �ۿ� Ȥ�� ������ ���� ��쿡�� Reliable ���
	// WithValidation: ���� �� ���� �۾�
	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnLandMine_Server();
	
	UFUNCTION(Server, Reliable)
	void SpawnWeaponInstance_Server();

	UFUNCTION(Server, Reliable)
	void DestroyWeaponInstance_Server();

	// ������
	virtual void OnRep_WeaponInstance() override;

	// Unreliable�� �� ����: 1 ~ 2�� ������ ������ �Ǳ� ����
	UFUNCTION(Server, Unreliable) 
	void UpdateInputValue_Server(const float& InForwardInputValue, const float& InRightInputValue);

	UFUNCTION(Server, Unreliable)
	void UpdateAimValue_Server(const float& InAimPitchValue, const float& InAimYawValue);

	UFUNCTION(Server, Unreliable)
	void PlayAttackMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackMontage_NetMulticast();

	// ���� Ʈ���̽� ������ Owner���� ����� ������ Server����
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
	// �߰��� ��⿡�� � ������ ���׸����� ������� �����ϱ� ����
	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath = FSoftObjectPath();

	// �ε��� �� �������� �ڵ�
	// �ڵ� ���� �˾ƾ��� �޽÷ε� ���� �ش� ������ ���� �� ����
	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	// ���� ������ ��Ÿ�� ���
	UPROPERTY()
	TObjectPtr<UAnimMontage> UnequipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<UCameraShakeBase> FireShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASViewCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<AActor> LandMineClass;

	// ���� ������ �ִϸ��̼� ���̾� ���
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

	// �д� źȯ�� �� �� �߻��� ���ΰ�
	// ���ڰ� Ŭ���� ������ ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float FirePerMinute = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

	// PlayerState ������ �̵�
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 CurrentKillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 MaxKillCount = 99;*/

	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	// �ǰ� ������ ���� Ÿ�̸� �ڵ�
	FTimerHandle HittedRagdollRestoreTimer;

	FTimerHandle BetweenShotsTimer;

	// UPROPERTY() ��ũ�θ� ������� �����Ƿ� �ʱ�ȭ�� �����ؾ���
	EViewMode CurrentViewMode = EViewMode::None;

	FVector DirectionToMove = FVector::ZeroVector;
	FRotator DestArmRotation = FRotator::ZeroRotator;

	float DestArmLength = 0.0f;
	float ArmLengthChangeSpeed = 3.0f;
	float ArmRotationChangeSpeed = 10.0f;

	// Zoom �۾�
	float TargetFOV = 70.0f;
	float CurrentFOV = 70.0f;

	// ���� �۾�
	float TimeBetweenFire;
	bool bIsTriggerToggle = false;

	// ���� ���� �޶����� �� Replicated �ϱ� ���� �۾�
	float PreviousForwardInputValue = 0.0f;
	float PreviousRightInputValue = 0.0f;
	float PreviousAimPitch = 0.0f;
	float PreviousAimYaw = 0.0f;

	// ���� ���� �۾�
	float TargetRagDollBlendWeight = 0.0f;
	float CurrentRagDollBlendWeight = 0.0f;
	bool bIsNowRagdollBlending = false;

};
