// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SCharacter.h"
#include "InputActionValue.h"
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

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnMeleeAttackMontageEnded(class UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnCheckHit();

	/*UFUNCTION()
	void OnCheckAttackInput();

	UFUNCTION()
	void EndCombo(UAnimMontage* InMontage, bool bInterruped);*/

	void SetViewMode(EViewMode InViewMode);

	//void BeginCombo();

	float GetForwardInputValue() const { return ForwardInputValue; }
	float GetRightInputValue() const { return RightInputValue; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void OnCheckAttackInput();

	UFUNCTION()
	void EndCombo(UAnimMontage* InMontage, bool bInterruped);

	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void ChangeView(const FInputActionValue& InValue);

	void InputQuickSlot01(const FInputActionValue& InValue);

	void InputQuickSlot02(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);

	void BeginCombo();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	// TSubclassOf: ASWeaponActor클래스를 기준으로 자식 클래스들이 선택 가능하게
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<class ASWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class ASWeaponActor> WeaponInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ForwardInputValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float RightInputValue;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;

	// UPROPERTY() 매크로를 사용하지 않으므로 초기화에 유념해야함
	EViewMode CurrentViewMode = EViewMode::None;

	FVector DirectionToMove = FVector::ZeroVector;
	FRotator DestArmRotation = FRotator::ZeroRotator;

	float DestArmLength = 0.0f;
	float ArmLengthChangeSpeed = 3.0f;
	float ArmRotationChangeSpeed = 10.0f;

	// 섹션별 어택 이름이 존재하기 때문에 이름을 받는 변수로 사용
	FString AttackAnimMontageSectionName = FString(TEXT("Attack"));
	int32 MaxComboCount = 3;
	int32 CurrentComboCount = 0;

	// 시리얼라이즈 되거나 레플리케이션 될 필요 없으므로 그냥 bool 자료형 사용
	// 해당 키가 들어와야 다음으로 넘어갈 수 있음
	bool bIsAttackKeyPressed = false;

	bool bIsNowAttacking = false;
};
