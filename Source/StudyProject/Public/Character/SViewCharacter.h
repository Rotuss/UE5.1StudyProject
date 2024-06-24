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

	void SetViewMode(EViewMode InViewMode);

	void SetMeshMaterial(const EPlayerTeam& InPlayerTeam);

	float GetForwardInputValue() const { return ForwardInputValue; }
	float GetRightInputValue() const { return RightInputValue; }

	UParticleSystemComponent* GetParticleSystem() const { return ParticleSystemComponent; }

	// PlayerState ������ �̵�
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ForwardInputValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float RightInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

	// PlayerState ������ �̵�
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 CurrentKillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 MaxKillCount = 99;*/

	// UPROPERTY() ��ũ�θ� ������� �����Ƿ� �ʱ�ȭ�� �����ؾ���
	EViewMode CurrentViewMode = EViewMode::None;

	FVector DirectionToMove = FVector::ZeroVector;
	FRotator DestArmRotation = FRotator::ZeroRotator;

	float DestArmLength = 0.0f;
	float ArmLengthChangeSpeed = 3.0f;
	float ArmRotationChangeSpeed = 10.0f;

};
