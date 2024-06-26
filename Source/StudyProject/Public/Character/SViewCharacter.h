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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TSubclassOf<UCameraShakeBase> FireShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ForwardInputValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float RightInputValue;

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

	float CurrentAimPitch = 0.0f;
	float CurrentAimYaw = 0.0f;

};
